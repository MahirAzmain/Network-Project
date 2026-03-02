#include "RPLNetworkLayer.h"

Define_Module(RPLNetworkLayer);

RPLNetworkLayer::RPLNetworkLayer()
    : nodeId(0), mopType(MOP0_NO_DOWNWARD),
      packetsForwarded(0), packetsDropped(0), packetsReceived(0) {}

RPLNetworkLayer::~RPLNetworkLayer() {}

void RPLNetworkLayer::initialize() {
    nodeId = getParentModule()->par("nodeId");
    mopType = (MOP)par("mopType").intValue();  // FIX: Use intValue() first, then cast
}


void RPLNetworkLayer::handleMessage(cMessage *msg) {
    if (msg->arrivedOn("upperLayerIn")) {
        handlePacketFromApp(msg);
    } else if (msg->arrivedOn("lowerLayerIn")) {
        handlePacketFromMac(msg);
    } else if (msg->arrivedOn("rplIn")) {
        handlePacketFromRPL(msg);
    }
}

void RPLNetworkLayer::handlePacketFromApp(cMessage *msg) {
    // Application data - send down to MAC
    send(msg, "lowerLayerOut");
}

void RPLNetworkLayer::handlePacketFromMac(cMessage *msg) {
    packetsReceived++;

    // Check if it's a control message (DIO/DIS/DAO)
    if (dynamic_cast<DIOMessage*>(msg) ||
        dynamic_cast<DISMessage*>(msg) ||
        dynamic_cast<DAOMessage*>(msg)) {
        // RPL control message - send to RPL module
        send(msg, "rplOut");
    } else if (dynamic_cast<DataPacket*>(msg)) {
        // Data packet - forward or deliver
        forwardPacket(msg);
    } else {
        // Unknown message type
        delete msg;
    }
}

void RPLNetworkLayer::handlePacketFromRPL(cMessage *msg) {
    // RPL control messages to be broadcasted
    send(msg, "lowerLayerOut");
}

void RPLNetworkLayer::forwardPacket(cMessage *msg) {
    DataPacket *pkt = dynamic_cast<DataPacket*>(msg);
    if (pkt) {
        // Simple forwarding - in real implementation, check routing table
        packetsForwarded++;
        send(msg, "lowerLayerOut");
    } else {
        packetsDropped++;
        delete msg;
    }
}

void RPLNetworkLayer::deliverToApp(cMessage *msg) {
    send(msg, "upperLayerOut");
}

void RPLNetworkLayer::finish() {
    recordScalar("packetsForwarded", packetsForwarded);
    recordScalar("packetsDropped", packetsDropped);
    recordScalar("packetsReceived", packetsReceived);
}
