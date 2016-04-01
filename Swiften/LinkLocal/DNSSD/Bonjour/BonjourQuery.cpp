/*
 * Copyright (c) 2010-2016 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#include <Swiften/LinkLocal/DNSSD/Bonjour/BonjourQuery.h>

#include <Swiften/LinkLocal/DNSSD/Bonjour/BonjourQuerier.h>

namespace Swift {

BonjourQuery::BonjourQuery(boost::shared_ptr<BonjourQuerier> q, EventLoop* eventLoop) : eventLoop(eventLoop), querier(q), sdRef(nullptr) {
}

BonjourQuery::~BonjourQuery() {
    DNSServiceRefDeallocate(sdRef);
}

void BonjourQuery::processResult() {
    boost::lock_guard<boost::mutex> lock(sdRefMutex);
    DNSServiceProcessResult(sdRef);
}

int BonjourQuery::getSocketID() const {
    boost::lock_guard<boost::mutex> lock(sdRefMutex);
    return DNSServiceRefSockFD(sdRef);
}

void BonjourQuery::run() {
    querier->addRunningQuery(shared_from_this());
}

void BonjourQuery::finish() {
    querier->removeRunningQuery(shared_from_this());
}

}
