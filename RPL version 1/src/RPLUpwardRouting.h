#ifndef __RPL_UPWARD_ROUTING_H_
#define __RPL_UPWARD_ROUTING_H_

#include <omnetpp.h>
#include "RPLCommon.h"
#include "RPLMessage.h"
#include "ParentTableRPL.h"
#include "RoutingTableRPL.h"
#include "SourceRoutingTable.h"

using namespace omnetpp;

class RPLUpwardRouting : public cSimpleModule {
  private:
    int nodeId;
    bool isRoot;
    MOP mopType;

    LinkLocalAddress *linkLocalAddr;
    GlobalAddress *globalAddr;
    IPv6Address dodagRootAddr;

    unsigned char instanceId;
    unsigned char versionNumber;

    ParentTableRPL *parentTable;
    RoutingTableRPL *routingTable;
    SourceRoutingTable *sourceRoutingTable;

    Rank currentRank;
    unsigned short dioIntMin;
    unsigned short dioIntMax;
    unsigned char dioRedundancy;

    cMessage *dioTimer;
    cMessage *disTimer;
    cMessage *daoTimer;

    long dioSent;
    long dioReceived;
    long daoSent;
    long daoReceived;

  public:
    RPLUpwardRouting();
    virtual ~RPLUpwardRouting();

  protected:
    virtual void initialize() override;
    virtual void handleMessage(cMessage *msg) override;
    virtual void finish() override;

  private:
    void processDIO(DIOMessage *dio);
    void sendDIO();
    void updateDIO(DIOMessage *dio);
    void selectParent(DIOMessage *dio);
    bool validateDIO(DIOMessage *dio);

    void processDIS(DISMessage *dis);
    void sendDIS();

    void processDAO(DAOMessage *dao);
    void sendDAO();
    void forwardDAO(DAOMessage *dao);

    Rank calculateRank(const Rank& parentRank);
    void generateAddresses();
    void addDefaultRoute(const IPv6Address& nextHop);
    void updateRoutingTable(const DAOMessage *dao);
    void updateSourceRoutingTable(const DAOMessage *dao);
    IPv6Address getPreferredNextHop();
};

#endif
