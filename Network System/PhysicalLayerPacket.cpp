#include "PhysicalLayerPacket.h"

PhysicalLayerPacket::PhysicalLayerPacket(int _layer_ID, const string& _sender_MAC, const string& _receiver_MAC)
        : Packet(_layer_ID) {
    sender_MAC_address = _sender_MAC;
    receiver_MAC_address = _receiver_MAC;
}

void PhysicalLayerPacket::print() {
    std::cout << "Frame #" << frame << std::endl;
    std::cout << "Sender MAC address: " << sender_MAC_address << ", Receiver MAC address: " << receiver_MAC_address << std::endl;
    // TODO: Override the virtual print function from Packet class to additionally print layer-specific properties.
}

void PhysicalLayerPacket::print_for_info() {
    std::cout << "Layer 3 info: Sender MAC address: " << sender_MAC_address << ", Receiver MAC address: " << receiver_MAC_address << std::endl;
    std::cout << "Number of hops so far: " << number_of_hops << std::endl;
}

PhysicalLayerPacket::~PhysicalLayerPacket() {
    // TODO: Free any dynamically allocated memory if necessary.
}