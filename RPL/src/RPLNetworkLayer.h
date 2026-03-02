#ifndef __RPL_NETWORK_LAYER_H_
#define __RPL_NETWORK_LAYER_H_

#include <omnetpp.h>
#include "RPLCommon.h"
#include "RPLMessage.h"

using namespace omnetpp;

class RPLNetworkLayer : public cSimpleModule {
  private:
    int nodeId;
    MOP mopType;

    long packetsForwarded;
    long packetsDropped;
    long packetsReceived;

  public:
    RPLNetworkLayer();
    virtual ~RPLNetworkLayer();

  protected:
    virtual void initialize() override;
    virtual void handleMessage(cMessage *msg) override;
    virtual void finish() override;

  private:
    void handlePacketFromApp(cMessage *msg);
    void handlePacketFromMac(cMessage *msg);
    void handlePacketFromRPL(cMessage *msg);
    void forwardPacket(cMessage *msg);
    void deliverToApp(cMessage *msg);
};

#endif
