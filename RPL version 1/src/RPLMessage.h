#ifndef __RPL_MESSAGE_H_
#define __RPL_MESSAGE_H_

#include <omnetpp.h>
#include "RPLCommon.h"

using namespace omnetpp;

// DIO (DODAG Information Object)
class DIOMessage : public cMessage {
  public:
    unsigned char instanceId;
    unsigned char version;
    unsigned short rank;
    unsigned char mop;
    unsigned char dioIntDoubling;
    unsigned char dioIntMin;
    unsigned char dioRedun;
    IPv6Address dodagId;
    IPv6Address sender;

    DIOMessage(const char *name = nullptr) : cMessage(name) {
        instanceId = DEFAULT_INSTANCE_ID;
        version = 0;
        rank = INFINITE_RANK;
        mop = MOP0_NO_DOWNWARD;
        dioIntDoubling = 8;
        dioIntMin = 12;
        dioRedun = 10;
    }

    DIOMessage *dup() const override {
        return new DIOMessage(*this);
    }
};

// DIS (DODAG Information Solicitation)
class DISMessage : public cMessage {
  public:
    unsigned char instanceId;
    IPv6Address dodagId;
    IPv6Address sender;

    DISMessage(const char *name = nullptr) : cMessage(name) {
        instanceId = DEFAULT_INSTANCE_ID;
    }

    DISMessage *dup() const override {
        return new DISMessage(*this);
    }
};

// DAO (Destination Advertisement Object)
class DAOMessage : public cMessage {
  public:
    unsigned char instanceId;
    unsigned char mop;
    IPv6Address dodagId;
    IPv6Address targetAddress;
    IPv6Address daoParent;
    IPv6Address sender;
    bool k_flag;

    DAOMessage(const char *name = nullptr) : cMessage(name) {
        instanceId = DEFAULT_INSTANCE_ID;
        k_flag = false;
    }

    DAOMessage *dup() const override {
        return new DAOMessage(*this);
    }
};

// Data Packet - Changed to cPacket to support setByteLength()
class DataPacket : public cPacket {
  public:
    IPv6Address source;
    IPv6Address destination;
    std::vector<IPv6Address> sourceRoute;
    int hopLimit;

    DataPacket(const char *name = nullptr) : cPacket(name), hopLimit(64) {}

    DataPacket *dup() const override {
        return new DataPacket(*this);
    }
};

#endif
