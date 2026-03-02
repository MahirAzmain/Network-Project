#include "IdealMac.h"

Define_Module(IdealMac);

IdealMac::IdealMac() : framesReceived(0), framesSent(0) {}

IdealMac::~IdealMac() {}

void IdealMac::initialize() {
    framesReceived = 0;
    framesSent = 0;
}

void IdealMac::handleMessage(cMessage *msg) {
    if (msg->arrivedOn("upperLayerIn")) {
        framesSent++;
        send(msg, "radioOut");
    } else if (msg->arrivedOn("radioIn")) {
        framesReceived++;
        send(msg, "upperLayerOut");
    }
}

void IdealMac::finish() {
    recordScalar("framesReceived", framesReceived);
    recordScalar("framesSent", framesSent);
}
