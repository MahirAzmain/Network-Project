#ifndef __IDEAL_MAC_H_
#define __IDEAL_MAC_H_

#include <omnetpp.h>

using namespace omnetpp;

class IdealMac : public cSimpleModule {
  private:
    long framesReceived;
    long framesSent;

  public:
    IdealMac();
    virtual ~IdealMac();

  protected:
    virtual void initialize() override;
    virtual void handleMessage(cMessage *msg) override;
    virtual void finish() override;
};

#endif
