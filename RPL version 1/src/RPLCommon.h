#ifndef __RPL_COMMON_H_
#define __RPL_COMMON_H_

#include <omnetpp.h>
#include <string>
#include <vector>
#include <map>
#include <cstring>
#include <sstream>
#include <iomanip>

using namespace omnetpp;

// Constants
#define DEFAULT_INSTANCE_ID 0
#define MAX_RANK 0xFFFF
#define INFINITE_RANK 0xFFFF
#define MIN_HOP_RANK_INCREASE 256

// MOP Types
enum MOP {
    MOP0_NO_DOWNWARD = 0,
    MOP1_NON_STORING = 1,
    MOP2_STORING = 2,
    MOP3_STORING_MULTICAST = 3
};

// IPv6 Address Structure
struct IPv6Address {
    unsigned char bytes[16];

    IPv6Address() { memset(bytes, 0, 16); }

    std::string toString() const {
        std::stringstream ss;
        for (int i = 0; i < 16; i++) {
            if (i > 0 && i % 2 == 0) ss << ":";
            ss << std::hex << std::setw(2) << std::setfill('0')
               << (int)bytes[i];
        }
        return ss.str();
    }

    static IPv6Address fromString(const std::string& str);

    bool operator==(const IPv6Address& other) const {
        return memcmp(bytes, other.bytes, 16) == 0;
    }

    bool operator!=(const IPv6Address& other) const {
        return !(*this == other);
    }
};

// Link Local Address (fe80::)
struct LinkLocalAddress : public IPv6Address {
    LinkLocalAddress(int nodeId) {
        memset(bytes, 0, 16);
        bytes[0] = 0xfe;
        bytes[1] = 0x80;
        bytes[8] = 0x0a;
        bytes[9] = 0x0a;
        bytes[10] = 0xff;
        bytes[11] = 0xfe;
        bytes[14] = (nodeId >> 8) & 0xFF;
        bytes[15] = nodeId & 0xFF;
    }
};

// Global Address (fd00::)
struct GlobalAddress : public IPv6Address {
    GlobalAddress(const LinkLocalAddress& linkLocal) {
        memset(bytes, 0, 16);  // Initialize all bytes
        bytes[0] = 0xfd;
        bytes[1] = 0x00;
        // Copy the interface identifier
        for (int i = 8; i < 16; i++) {
            bytes[i] = linkLocal.bytes[i];
        }
    }
};

// Rank Structure
struct Rank {
    unsigned short value;
    unsigned char flags;
    unsigned char mopSupported;

    Rank() : value(INFINITE_RANK), flags(0), mopSupported(0) {}
    Rank(unsigned short v) : value(v), flags(0), mopSupported(0) {}
};

// Parent Information
struct ParentInfo {
    IPv6Address address;
    Rank rank;
    unsigned short metric;
    int sequenceNumber;
    simtime_t lastUpdate;
    bool preferred;

    ParentInfo() : metric(0), sequenceNumber(0),
                   lastUpdate(0), preferred(false) {}
};

// Routing Table Entry
struct RoutingTableEntry {
    IPv6Address destination;
    IPv6Address nextHop;
    int hopCount;
    bool isDefaultRoute;

    RoutingTableEntry() : hopCount(0), isDefaultRoute(false) {}
};

// Source Routing Entry (MOP1)
struct SourceRoutingEntry {
    IPv6Address child;
    IPv6Address daoParent;
    std::vector<IPv6Address> route;

    SourceRoutingEntry() {}
};

#endif
