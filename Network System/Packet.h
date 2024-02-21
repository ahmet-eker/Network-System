#ifndef PACKET_H
#define PACKET_H

#include <string>
#include <iostream>

using namespace std;

class Packet {
public:
    Packet(int layerID);
    virtual ~Packet();

    int layer_ID;
    int frame;
    int number_of_hops = 0;
    int number_of_frames;

    friend ostream &operator<<(ostream &os, const Packet &packet);
    virtual void print() {};
    virtual void print_for_info() {};
};

#endif // PACKET_H