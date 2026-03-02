#ifndef __APP_LAYER_H_
#define __APP_LAYER_H_

#include <omnetpp.h>

using namespace omnetpp;

class AppLayer : public cSimpleModule {
  private:
    cMessage *trafficTimer;
    int nodeId;
    double sendInterval;
    long packetsSent;
    long packetsReceived;

  public:
    AppLayer();
    virtual ~AppLayer();

  protected:
    virtual void initialize() override;
    virtual void handleMessage(cMessage *msg) override;
    virtual void finish() override;

  private:
    void generateTraffic();
};

#endif
