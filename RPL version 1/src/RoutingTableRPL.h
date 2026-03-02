#ifndef __ROUTING_TABLE_RPL_H_
#define __ROUTING_TABLE_RPL_H_

#include "RPLCommon.h"
#include <vector>

class RoutingTableRPL {
  private:
    std::vector<RoutingTableEntry> entries;

  public:
    RoutingTableRPL();
    virtual ~RoutingTableRPL();

    void addRoute(const RoutingTableEntry& entry);
    void updateRoute(const RoutingTableEntry& entry);
    void removeRoute(const IPv6Address& destination);
    RoutingTableEntry* getRoute(const IPv6Address& destination);
    RoutingTableEntry* getDefaultRoute();
    std::vector<RoutingTableEntry>& getEntries() { return entries; }
    bool hasRoute(const IPv6Address& destination) const;
    void clear();
    int size() const { return entries.size(); }
};

#endif
