#include "AppLayer.h"
#include "RPLMessage.h"

Define_Module(AppLayer);

AppLayer::AppLayer() : trafficTimer(nullptr), nodeId(0),
                       sendInterval(10.0),
                       packetsSent(0), packetsReceived(0) {}

AppLayer::~AppLayer() {
    cancelAndDelete(trafficTimer);
}

void AppLayer::initialize() {
    nodeId = getParentModule()->par("nodeId");
    sendInterval = par("sendInterval");
    packetsSent = 0;
    packetsReceived = 0;

    if (nodeId != 0) {
        trafficTimer = new cMessage("trafficTimer");
        scheduleAt(simTime() + sendInterval + uniform(0, 1), trafficTimer);
    }
}

void AppLayer::handleMessage(cMessage *msg) {
    if (msg == trafficTimer) {
        generateTraffic();
        scheduleAt(simTime() + sendInterval, trafficTimer);
    } else if (msg->arrivedOn("networkIn")) {
        packetsReceived++;
        EV << "App layer at node " << nodeId
           << " received packet" << endl;
        delete msg;
    }
}

void AppLayer::generateTraffic() {
    DataPacket *pkt = new DataPacket("dataPacket");
    pkt->setByteLength(par("dataPacketSize"));
    pkt->hopLimit = 64;

    send(pkt, "networkOut");
    packetsSent++;

    EV << "App layer at node " << nodeId
       << " generated packet" << endl;
}


void AppLayer::finish() {
    recordScalar("packetsSent", packetsSent);
    recordScalar("packetsReceived", packetsReceived);
}
