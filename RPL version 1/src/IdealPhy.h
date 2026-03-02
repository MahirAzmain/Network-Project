#ifndef __IDEAL_PHY_H_
#define __IDEAL_PHY_H_

#include <omnetpp.h>

using namespace omnetpp;

class IdealPhy : public cSimpleModule {
  private:
    double transmissionRange;
    long packetsReceived;
    long packetsSent;

  public:
    IdealPhy();
    virtual ~IdealPhy();

  protected:
    virtual void initialize() override;
    virtual void handleMessage(cMessage *msg) override;
    virtual void finish() override;

  private:
    void broadcastPacket(cMessage *msg);
};

#endif
