#ifndef __PARENT_TABLE_RPL_H_
#define __PARENT_TABLE_RPL_H_

#include "RPLCommon.h"
#include <map>
#include <vector>

class ParentTableRPL {
  private:
    std::map<std::string, ParentInfo> parents;
    ParentInfo *preferredParent;

  public:
    ParentTableRPL();
    virtual ~ParentTableRPL();

    void addParent(const ParentInfo& parent);
    void updateParent(const ParentInfo& parent);
    void removeParent(const IPv6Address& address);
    ParentInfo* getParent(const IPv6Address& address);
    ParentInfo* getPreferredParent() const { return preferredParent; }
    void setPreferredParent(const IPv6Address& address);
    std::vector<ParentInfo*> getAllParents();
    bool hasParent(const IPv6Address& address) const;
    void clear();
    int size() const { return parents.size(); }
};

#endif
