#ifndef __SOURCE_ROUTING_TABLE_H_
#define __SOURCE_ROUTING_TABLE_H_

#include "RPLCommon.h"
#include <map>

class SourceRoutingTable {
  private:
    std::map<std::string, SourceRoutingEntry> routes;

  public:
    SourceRoutingTable();
    virtual ~SourceRoutingTable();

    void addRoute(const SourceRoutingEntry& entry);
    void updateRoute(const SourceRoutingEntry& entry);
    void removeRoute(const IPv6Address& child);
    SourceRoutingEntry* getRoute(const IPv6Address& child);
    bool hasRoute(const IPv6Address& child) const;
    void clear();
    int size() const { return routes.size(); }
};

#endif
