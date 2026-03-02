#include "SourceRoutingTable.h"

SourceRoutingTable::SourceRoutingTable() {}

SourceRoutingTable::~SourceRoutingTable() {
    clear();
}

void SourceRoutingTable::addRoute(const SourceRoutingEntry& entry) {
    std::string key = entry.child.toString();
    routes[key] = entry;
}

void SourceRoutingTable::updateRoute(const SourceRoutingEntry& entry) {
    addRoute(entry);
}

void SourceRoutingTable::removeRoute(const IPv6Address& child) {
    std::string key = child.toString();
    routes.erase(key);
}

SourceRoutingEntry* SourceRoutingTable::getRoute(const IPv6Address& child) {
    std::string key = child.toString();
    auto it = routes.find(key);
    if (it != routes.end()) {
        return &it->second;
    }
    return nullptr;
}

bool SourceRoutingTable::hasRoute(const IPv6Address& child) const {
    std::string key = child.toString();
    return routes.find(key) != routes.end();
}

void SourceRoutingTable::clear() {
    routes.clear();
}
