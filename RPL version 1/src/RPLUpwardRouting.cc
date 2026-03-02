#include "RPLUpwardRouting.h"

Define_Module(RPLUpwardRouting);

RPLUpwardRouting::RPLUpwardRouting()
    : nodeId(0), isRoot(false), mopType(MOP0_NO_DOWNWARD),
      linkLocalAddr(nullptr), globalAddr(nullptr),
      instanceId(DEFAULT_INSTANCE_ID), versionNumber(0),
      parentTable(nullptr), routingTable(nullptr),
      sourceRoutingTable(nullptr),
      dioTimer(nullptr), disTimer(nullptr), daoTimer(nullptr),
      dioSent(0), dioReceived(0), daoSent(0), daoReceived(0) {}

RPLUpwardRouting::~RPLUpwardRouting() {
    cancelAndDelete(dioTimer);
    cancelAndDelete(disTimer);
    cancelAndDelete(daoTimer);
    delete linkLocalAddr;
    delete globalAddr;
    delete parentTable;
    delete routingTable;
    delete sourceRoutingTable;
}

void RPLUpwardRouting::initialize() {
    nodeId = getParentModule()->par("nodeId");
    isRoot = (nodeId == 0);
    mopType = (MOP)par("mopType").intValue();

    generateAddresses();

    parentTable = new ParentTableRPL();
    routingTable = new RoutingTableRPL();
    sourceRoutingTable = new SourceRoutingTable();

    dioIntMin = 12;
    dioIntMax = 20;
    dioRedundancy = 10;

    if (isRoot) {
        currentRank = Rank(MIN_HOP_RANK_INCREASE);
        dodagRootAddr = *globalAddr;
        versionNumber = 1;  // Root starts with version 1
    } else {
        currentRank = Rank(INFINITE_RANK);
    }

    if (isRoot) {
        dioTimer = new cMessage("dioTimer", 1);
        scheduleAt(simTime() + 1.0, dioTimer);
    } else {
        disTimer = new cMessage("disTimer", 2);
        scheduleAt(simTime() + uniform(0.1, 0.5), disTimer);
    }

    EV << "RPLUpwardRouting initialized for node " << nodeId
       << " with address " << linkLocalAddr->toString() << endl;
}

void RPLUpwardRouting::generateAddresses() {
    linkLocalAddr = new LinkLocalAddress(nodeId);
    globalAddr = new GlobalAddress(*linkLocalAddr);
}

void RPLUpwardRouting::handleMessage(cMessage *msg) {
    if (msg == dioTimer) {
        sendDIO();
        scheduleAt(simTime() + 8.0, dioTimer);
    } else if (msg == disTimer) {
        if (parentTable->size() == 0) {
            sendDIS();
            scheduleAt(simTime() + 2.0, disTimer);
        } else {
            cancelEvent(disTimer);
        }
    } else if (msg == daoTimer) {
        sendDAO();
        scheduleAt(simTime() + 10.0, daoTimer);
    } else if (msg->arrivedOn("netIn")) {
        if (DIOMessage *dio = dynamic_cast<DIOMessage*>(msg)) {
            processDIO(dio);
        } else if (DISMessage *dis = dynamic_cast<DISMessage*>(msg)) {
            processDIS(dis);
        } else if (DAOMessage *dao = dynamic_cast<DAOMessage*>(msg)) {
            processDAO(dao);
        }
        delete msg;
    }
}

void RPLUpwardRouting::processDIO(DIOMessage *dio) {
    dioReceived++;

    if (!validateDIO(dio)) {
        EV << "Invalid DIO received at node " << nodeId << endl;
        return;
    }

    // Update DODAG version if necessary
    if (isRoot) {
        return;  // Root doesn't process DIO from others
    }

    if (dio->version > versionNumber) {
        versionNumber = dio->version;
        dodagRootAddr = dio->dodagId;
    }

    selectParent(dio);

    // Schedule DAO timer if not already scheduled
    if (mopType != MOP0_NO_DOWNWARD && !daoTimer) {
        daoTimer = new cMessage("daoTimer", 3);
        scheduleAt(simTime() + 3.0, daoTimer);
    }

    // Forward updated DIO
    if (currentRank.value < INFINITE_RANK && !dioTimer) {
        dioTimer = new cMessage("dioTimer", 1);
        scheduleAt(simTime() + uniform(1.0, 2.0), dioTimer);
    }
}

void RPLUpwardRouting::processDIS(DISMessage *dis) {
    if (currentRank.value < INFINITE_RANK) {
        // Respond with DIO
        DIOMessage *dio = new DIOMessage("DIO");
        dio->instanceId = instanceId;
        dio->version = versionNumber;
        dio->rank = currentRank.value;
        dio->mop = mopType;
        dio->dodagId = dodagRootAddr;
        dio->sender = *linkLocalAddr;

        send(dio, "netOut");
        dioSent++;
    }
}

void RPLUpwardRouting::processDAO(DAOMessage *dao) {
    daoReceived++;

    if (isRoot) {
        if (mopType == MOP1_NON_STORING) {
            updateSourceRoutingTable(dao);
        } else if (mopType == MOP2_STORING) {
            updateRoutingTable(dao);
        }
    } else if (mopType == MOP2_STORING) {
        updateRoutingTable(dao);
        forwardDAO(dao);
    } else {
        forwardDAO(dao);
    }
}

void RPLUpwardRouting::sendDIO() {
    if (currentRank.value >= INFINITE_RANK && !isRoot) {
        return;  // Don't send DIO if not part of DODAG
    }

    DIOMessage *dio = new DIOMessage("DIO");
    dio->instanceId = instanceId;
    dio->version = versionNumber;
    dio->rank = currentRank.value;
    dio->mop = mopType;
    dio->dodagId = dodagRootAddr;
    dio->sender = *linkLocalAddr;

    send(dio, "netOut");
    dioSent++;
}

void RPLUpwardRouting::sendDIS() {
    DISMessage *dis = new DISMessage("DIS");
    dis->instanceId = instanceId;
    dis->sender = *linkLocalAddr;

    send(dis, "netOut");
}

void RPLUpwardRouting::sendDAO() {
    if (parentTable->getPreferredParent() == nullptr) return;
    if (currentRank.value >= INFINITE_RANK) return;

    DAOMessage *dao = new DAOMessage("DAO");
    dao->instanceId = instanceId;
    dao->mop = mopType;
    dao->dodagId = dodagRootAddr;
    dao->targetAddress = *globalAddr;
    dao->daoParent = parentTable->getPreferredParent()->address;
    dao->sender = *linkLocalAddr;

    send(dao, "netOut");
    daoSent++;
}

void RPLUpwardRouting::selectParent(DIOMessage *dio) {
    // Only select parent if the rank is better
    if (dio->rank >= INFINITE_RANK) return;

    Rank newRank = calculateRank(Rank(dio->rank));
    if (newRank.value >= currentRank.value && currentRank.value < INFINITE_RANK) {
        return;  // Current parent is better or equal
    }

    ParentInfo parent;
    parent.address = dio->sender;
    parent.rank = Rank(dio->rank);
    parent.lastUpdate = simTime();
    parent.metric = 1;

    parentTable->updateParent(parent);
    parentTable->setPreferredParent(dio->sender);

    currentRank = newRank;
    addDefaultRoute(dio->sender);

    EV << "Node " << nodeId << " selected parent " << dio->sender.toString()
       << " with rank " << currentRank.value << endl;
}

bool RPLUpwardRouting::validateDIO(DIOMessage *dio) {
    if (dio->instanceId != instanceId) return false;
    if (dio->rank >= INFINITE_RANK) return false;
    if (dio->sender == *linkLocalAddr) return false;  // Don't process own DIO
    return true;
}

void RPLUpwardRouting::updateDIO(DIOMessage *dio) {
    dio->rank = currentRank.value;
    dio->sender = *linkLocalAddr;
}

Rank RPLUpwardRouting::calculateRank(const Rank& parentRank) {
    unsigned short newRank = parentRank.value + MIN_HOP_RANK_INCREASE;
    if (newRank > INFINITE_RANK) newRank = INFINITE_RANK;
    return Rank(newRank);
}

void RPLUpwardRouting::addDefaultRoute(const IPv6Address& nextHop) {
    RoutingTableEntry entry;
    entry.destination = dodagRootAddr;
    entry.nextHop = nextHop;
    entry.hopCount = 1;
    entry.isDefaultRoute = true;

    routingTable->addRoute(entry);
}

void RPLUpwardRouting::updateSourceRoutingTable(const DAOMessage *dao) {
    SourceRoutingEntry entry;
    entry.child = dao->targetAddress;
    entry.daoParent = dao->daoParent;

    sourceRoutingTable->addRoute(entry);

    EV << "Source route added at root for " << dao->targetAddress.toString() << endl;
}

void RPLUpwardRouting::updateRoutingTable(const DAOMessage *dao) {
    RoutingTableEntry entry;
    entry.destination = dao->targetAddress;
    entry.nextHop = dao->sender;
    entry.hopCount = 1;
    entry.isDefaultRoute = false;

    routingTable->addRoute(entry);

    EV << "Route added for " << dao->targetAddress.toString()
       << " via " << dao->sender.toString() << endl;
}

void RPLUpwardRouting::forwardDAO(DAOMessage *dao) {
    auto parent = parentTable->getPreferredParent();
    if (parent != nullptr) {
        DAOMessage *newDao = dao->dup();
        newDao->sender = *linkLocalAddr;
        send(newDao, "netOut");
        daoSent++;
    }
}

IPv6Address RPLUpwardRouting::getPreferredNextHop() {
    auto parent = parentTable->getPreferredParent();
    if (parent != nullptr) {
        return parent->address;
    }
    return IPv6Address();
}

void RPLUpwardRouting::finish() {
    recordScalar("dioSent", dioSent);
    recordScalar("dioReceived", dioReceived);
    recordScalar("daoSent", daoSent);
    recordScalar("daoReceived", daoReceived);
    recordScalar("parentTableSize", parentTable->size());
    recordScalar("routingTableSize", routingTable->size());
    recordScalar("finalRank", currentRank.value);
}
