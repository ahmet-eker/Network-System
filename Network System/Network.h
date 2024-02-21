#ifndef NETWORK_H
#define NETWORK_H

#include <vector>
#include <iostream>
#include "Packet.h"
#include "Client.h"
#define PRINT(x) std::cout << x << std::endl

using namespace std;

class Network {
public:
    Network();
    ~Network();

    int messageLimit;
    string senderPort;
    string receiverPort;
    int number_of_clients;
    int number_of_commands;
    vector<Client> clients_vector;

    // Executes commands given as a vector of strings while utilizing the remaining arguments.
    void process_commands(vector<Client> &clients, vector<string> &commands, int message_limit, const string &sender_port,
                     const string &receiver_port);

    // Initialize the network from the input files.
    vector<Client> read_clients(string const &filename);
    void read_routing_tables(vector<Client> &clients, string const &filename);
    vector<string> read_commands(const string &filename);

    void message_command(vector<string> commandline, string &message, vector<Client> &clients);
    void show_frame_info_command(vector<string> commandline, vector<Client> &clients);
    void show_queue_info_command(vector<string> commandline, vector<Client> &clients);


    void print_packages();
    void print_header(string command_line);

    string repeatString(string& str, int count);
    Client* return_client(string layer_ID, vector<Client> &clients);
    vector<string> split_string(string &message, char seperator);
    stack<Packet*> getQueueElementAtIndex(const std::queue<stack<Packet*>>& q, string index);
    Packet* getIndexElement(const std::stack<Packet*>& stk, int index);
    stack<Packet*> reverseStack(stack<Packet*>& originalStack);

};

#endif  // NETWORK_H
