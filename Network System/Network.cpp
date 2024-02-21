#include "Network.h"
#include "Client.h"
#include <fstream>
#include <sstream>
#include <chrono>
#include <iomanip>

Network::Network() {

}

void Network::process_commands(vector<Client> &clients, vector<string> &commands, int message_limit,
                      const string &sender_port, const string &receiver_port) {
    messageLimit = message_limit;
    senderPort = sender_port;
    receiverPort = receiver_port;

    for(string command_line : commands){
        vector<string> elements = split_string(command_line, ' ');
        string command = elements[0];

        print_header(command_line);

        if(command == "MESSAGE"){

            vector<string> h_elements = split_string(command_line, '#');
            std::cout << "Message to be sent: \"" << h_elements[1] << "\"\n" << std::endl;
            string message = h_elements[1];

            message_command(elements,message,clients);

        }else if(command == "SHOW_FRAME_INFO"){

            show_frame_info_command(elements,clients);

        }else if(command == "SHOW_Q_INFO"){

            show_queue_info_command(elements,clients);

        }else if(command == "SEND"){
            //PRINT(" ");
        }else if(command == "RECEIVE"){
            //PRINT(" ");
        }else if(command == "PRINT_LOG"){
            //PRINT(" ");
        }else {
            PRINT("Invalid Command");
        }
    }

    // TODO: Execute the commands given as a vector of strings while utilizing the remaining arguments.
    /* Don't use any static variables, assume this method will be called over and over during testing.
     Don't forget to update the necessary member variables after processing each command. For example,
     after the MESSAGE command, the outgoing queue of the sender must have the expected frames ready to send. */
}

vector<Client> Network::read_clients(const string &filename) {
    vector<Client> clients;

    string filePath = filename;
    ifstream inputFile(filePath);

    vector<string> lines;
    string line;
    while (std::getline(inputFile, line)) {
        lines.push_back(line);
    }

    inputFile.close();

    number_of_clients = stoi(lines[0]);
    lines.erase(lines.begin());

    for(string line : lines){
        istringstream iss(line);

        vector<string> elements;
        string element;
        while (std::getline(iss, element, ' ')) {
            elements.push_back(element);
        }

        Client* newclient = new Client(elements[0],elements[1],elements[2]);
        clients.push_back(*newclient);
    }

    clients_vector = clients;
    return clients;
}

void Network::read_routing_tables(vector<Client> &clients, const string &filename) {

    string filePath = filename;
    ifstream inputFile(filePath);

    vector<string> lines;
    string line;
    int index = 0;
    while (std::getline(inputFile, line)) {
        if(line == "-"){
            index += 1;
        }else{
            lines.push_back(line);
            istringstream iss(line);

            vector<string> elements;
            string element;
            while (std::getline(iss, element, ' ')) {
                elements.push_back(element);
            }
            clients[index].routing_table[elements[0]] = elements[1];
        }
    }
    inputFile.close();
    // TODO: Read the routing tables from the given input file and populate the clients' routing_table member variable.
}

vector<string> Network::read_commands(const string &filename) {
    vector<string> commands;

    string filePath = filename;
    ifstream inputFile(filePath);

    string line;
    while (std::getline(inputFile, line)) {
        commands.push_back(line);
    }

    number_of_commands = stoi(commands[0]);
    commands.erase(commands.begin());

    inputFile.close();
    // TODO: Read commands from the given input file and return them as a vector of strings.
    return commands;
}

Network::~Network() {
    // TODO: Free any dynamically allocated memory if necessary.
}

void Network::message_command(vector<string> commandline, string &message, vector<Client> &clients){

    string sender_ID_p = commandline[1];
    string receiver_ID_p = commandline[2];
    string sender_port_number_p = senderPort;
    string receiver_port_number_p = receiverPort;
    string sender_IP_p;
    string receiver_IP_p;
    string sender_MAC_p;
    string reciever_MAC_p;

    int number_of_frames_p = message.length() / (messageLimit + 1) + 1;
    int number_of_hops_p = 0;

    std::stack<Packet*> packet_stack;

    Client* sender_client = return_client(sender_ID_p, clients);
    Client* receiver_client = return_client(receiver_ID_p, clients);

    string first_reciever_id = sender_client->routing_table[receiver_ID_p];
    Client* after_client = return_client(first_reciever_id,clients);

    sender_IP_p = sender_client->client_ip;
    sender_MAC_p = sender_client->client_mac;

    receiver_IP_p = receiver_client->client_ip;
    reciever_MAC_p = after_client->client_mac;


    int frame_p = 1;
    for (int i = 0; i < message.length(); i += messageLimit) {
        string part = message.substr(i, messageLimit);
        ApplicationLayerPacket* applicationLayerPacket = new ApplicationLayerPacket(0,sender_ID_p,receiver_ID_p,part);
        applicationLayerPacket->number_of_hops = number_of_hops_p;
        packet_stack.push(applicationLayerPacket);
        TransportLayerPacket* transportLayerPacket = new TransportLayerPacket(1,sender_port_number_p,receiver_port_number_p);
        packet_stack.push(transportLayerPacket);
        NetworkLayerPacket* networkLayerPacket = new NetworkLayerPacket(2,sender_IP_p,receiver_IP_p);
        packet_stack.push(networkLayerPacket);
        PhysicalLayerPacket* physicalLayerPacket = new PhysicalLayerPacket(3,sender_MAC_p,reciever_MAC_p);
        physicalLayerPacket->number_of_frames = number_of_frames_p;
        physicalLayerPacket->frame = frame_p;
        packet_stack.push(physicalLayerPacket);
        frame_p++;

        sender_client->outgoing_queue.push(packet_stack);

        while (!packet_stack.empty()) {
            packet_stack.top()->print();
            packet_stack.pop();
        }
    }

    auto now = std::chrono::system_clock::now();
    std::time_t currentTime = std::chrono::system_clock::to_time_t(now);
    std::stringstream ss;
    ss << std::put_time(std::localtime(&currentTime), "%Y-%m-%d %H:%M:%S");
    std::string timestamp = ss.str();


    Log* new_log = new Log(timestamp, message, number_of_frames_p, number_of_hops_p, sender_ID_p, receiver_ID_p, true, ActivityType::MESSAGE_SENT);

    sender_client->log_entries.push_back(*new_log);
}


void Network::show_frame_info_command(vector<string> commandline, vector<Client> &clients) {
    /*
    string layer_id_p = commandline[1];
    string direction = commandline[2];
    string frame_number = commandline[3];

    string direction_long;

    stack<Packet*> result_stack;


    Client* info_client = return_client(layer_id_p, clients);
    queue<stack<Packet*>> copy_queue;
    if(direction == "in"){
        copy_queue = info_client->incoming_queue;
        direction_long = "Incoming";
    }else if(direction == "out"){
        copy_queue = info_client->outgoing_queue;
        direction_long = "Outgoing";
    }

    if(copy_queue.size() >= std::stoi(frame_number)){
        result_stack = getQueueElementAtIndex(copy_queue, frame_number);
    }else{
        PRINT("No such frame.");
        return;
    }

    std::cout << "Current Frame #" << frame_number << " on the " << direction_long << " queue of client " << layer_id_p << std::endl;

    stack<Packet*> result_stack_reverse = reverseStack(result_stack);

    while (!result_stack_reverse.empty()) {
        result_stack_reverse.top()->print_for_info();
        result_stack_reverse.pop();
    }*/
}


void Network::show_queue_info_command(vector<string> commandline, vector<Client> &clients){
/*
    string layer_id_p = commandline[1];
    string direction = commandline[2];
    Client* info_client = return_client(layer_id_p, clients);
    string direction_long;
    queue<stack<Packet*>> copy_queue;

    if(direction == "in"){
        copy_queue = info_client->incoming_queue;
        direction_long = "Incoming";
    }else if(direction == "out"){
        copy_queue = info_client->outgoing_queue;
        direction_long = "Outgoing";
    }

    int frame_number = copy_queue.size();

    std::cout << "Client " << layer_id_p << " " << direction_long << " Queue Status" << std::endl;
    std::cout << "Current total number of frames: " << frame_number << std::endl;
*/
}




string Network::repeatString(string& str, int count) {
    string result;
    for (int i = 0; i < count; ++i) {
        result += str;
    }
    return result;
}

void Network::print_header(string command_line) {
    string tire = "-";
    string msg = "Command: " + command_line;
    string hyphen = repeatString(tire, msg.size());
    PRINT(hyphen);
    PRINT(msg);
    PRINT(hyphen);
}

Client* Network::return_client(string layer_ID, vector<Client> &clients){
    for(auto &client : clients){
        if(client.client_id == layer_ID){
            return &client;
        }
    }
}

vector<string> Network::split_string(string &message, char seperator){

    std::istringstream iss(message);
    std::vector<std::string> elements;
    std::string element;

    while (std::getline(iss, element, seperator)) {
        elements.push_back(element);
    }

    return elements;
}

stack<Packet*> Network::getQueueElementAtIndex(const std::queue<stack<Packet*>>& q, string index) {
    int int_index = std::stoi(index);
    std::queue<stack<Packet*>> tempQueue = q;
    for (size_t i = 0; i < int_index; ++i) {
        tempQueue.pop();
    }
    return tempQueue.front();
}

Packet* Network::getIndexElement(const std::stack<Packet*>& stk, int index) {
    std::stack<Packet*> tempStack = stk;
    while (tempStack.size() > (index + 1)) {
        tempStack.pop();
    }
    return tempStack.top();
}

stack<Packet*> Network::reverseStack(stack<Packet*>& originalStack) {
    stack<Packet*> auxiliaryStack;

    while (!originalStack.empty()) {
        Packet* element = originalStack.top();
        originalStack.pop();
        auxiliaryStack.push(element);
    }

    return auxiliaryStack;
}