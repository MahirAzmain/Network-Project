#include "IdealPhy.h"

Define_Module(IdealPhy);

IdealPhy::IdealPhy() : transmissionRange(100),
                       packetsReceived(0), packetsSent(0) {}

IdealPhy::~IdealPhy() {}

void IdealPhy::initialize() {
    transmissionRange = par("transmissionRange");
    packetsReceived = 0;
    packetsSent = 0;
}

void IdealPhy::handleMessage(cMessage *msg) {
    if (msg->arrivedOn("radioIn")) {
        packetsSent++;
        broadcastPacket(msg);
    }
}

void IdealPhy::broadcastPacket(cMessage *msg) {
    cModule *parentNode = getParentModule();
    cModule *networkModule = parentNode->getParentModule();

    if (networkModule != nullptr) {
        for (cModule::SubmoduleIterator it(networkModule); !it.end(); ++it) {
            cModule *node = *it;
            if (node != parentNode) {
                cModule *targetPhy = node->getSubmodule("phy");
                if (targetPhy != nullptr) {
                    cMessage *copy = msg->dup();
                    sendDirect(copy, targetPhy, "radioOut");
                }
            }
        }
    }

    delete msg;
}

void IdealPhy::finish() {
    recordScalar("packetsReceived", packetsReceived);
    recordScalar("packetsSent", packetsSent);
}
