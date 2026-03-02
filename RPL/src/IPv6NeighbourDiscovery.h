#ifndef __IPV6_NEIGHBOUR_DISCOVERY_H_
#define __IPV6_NEIGHBOUR_DISCOVERY_H_

#include <omnetpp.h>
using namespace omnetpp;

class IPv6NeighbourDiscovery : public cSimpleModule
{
  protected:
    virtual void initialize() override { }
    virtual void handleMessage(cMessage *msg) override { delete msg; }
};

#endif
