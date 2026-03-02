#include "RoutingTableRPL.h"

RoutingTableRPL::RoutingTableRPL() {}

RoutingTableRPL::~RoutingTableRPL() {
    clear();
}

void RoutingTableRPL::addRoute(const RoutingTableEntry& entry) {
    for (auto& e : entries) {
        if (e.destination == entry.destination) {
            e = entry;
            return;
        }
    }
    entries.push_back(entry);
}

void RoutingTableRPL::updateRoute(const RoutingTableEntry& entry) {
    addRoute(entry);
}

void RoutingTableRPL::removeRoute(const IPv6Address& destination) {
    for (auto it = entries.begin(); it != entries.end(); ++it) {
        if (it->destination == destination) {
            entries.erase(it);
            break;
        }
    }
}

RoutingTableEntry* RoutingTableRPL::getRoute(const IPv6Address& destination) {
    for (auto& e : entries) {
        if (e.destination == destination) {
            return &e;
        }
    }
    return nullptr;
}

RoutingTableEntry* RoutingTableRPL::getDefaultRoute() {
    for (auto& e : entries) {
        if (e.isDefaultRoute) {
            return &e;
        }
    }
    return nullptr;
}

bool RoutingTableRPL::hasRoute(const IPv6Address& destination) const {
    for (const auto& e : entries) {
        if (e.destination == destination) {
            return true;
        }
    }
    return false;
}

void RoutingTableRPL::clear() {
    entries.clear();
}
