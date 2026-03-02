#include "ParentTableRPL.h"

ParentTableRPL::ParentTableRPL() : preferredParent(nullptr) {}

ParentTableRPL::~ParentTableRPL() {
    clear();
}

void ParentTableRPL::addParent(const ParentInfo& parent) {
    std::string key = parent.address.toString();
    parents[key] = parent;
    if (preferredParent == nullptr) {
        preferredParent = &parents[key];
    }
}

void ParentTableRPL::updateParent(const ParentInfo& parent) {
    std::string key = parent.address.toString();
    if (parents.find(key) != parents.end()) {
        parents[key] = parent;
        // Update preferredParent pointer if it was this parent
        if (preferredParent != nullptr && preferredParent->address == parent.address) {
            preferredParent = &parents[key];
        }
    } else {
        addParent(parent);
    }
}

void ParentTableRPL::removeParent(const IPv6Address& address) {
    std::string key = address.toString();
    parents.erase(key);
    if (preferredParent != nullptr && preferredParent->address == address) {
        preferredParent = nullptr;
    }
}

ParentInfo* ParentTableRPL::getParent(const IPv6Address& address) {
    std::string key = address.toString();
    auto it = parents.find(key);
    if (it != parents.end()) {
        return &it->second;
    }
    return nullptr;
}

void ParentTableRPL::setPreferredParent(const IPv6Address& address) {
    auto parent = getParent(address);
    if (parent != nullptr) {
        preferredParent = parent;
        preferredParent->preferred = true;
    }
}

std::vector<ParentInfo*> ParentTableRPL::getAllParents() {
    std::vector<ParentInfo*> allParents;
    for (auto& p : parents) {
        allParents.push_back(&p.second);
    }
    return allParents;
}

bool ParentTableRPL::hasParent(const IPv6Address& address) const {
    std::string key = address.toString();
    return parents.find(key) != parents.end();
}

void ParentTableRPL::clear() {
    parents.clear();
    preferredParent = nullptr;
}
