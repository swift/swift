/*
 * Copyright (c) 2010-2016 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#include <Swiften/LinkLocal/DNSSD/Fake/FakeDNSSDQuery.h>

#include <Swiften/LinkLocal/DNSSD/Fake/FakeDNSSDQuerier.h>

namespace Swift {

FakeDNSSDQuery::FakeDNSSDQuery(std::shared_ptr<FakeDNSSDQuerier> querier) : querier(querier) {
}

FakeDNSSDQuery::~FakeDNSSDQuery() {
}

void FakeDNSSDQuery::run() {
    querier->addRunningQuery(shared_from_this());
}

void FakeDNSSDQuery::finish() {
    querier->removeRunningQuery(shared_from_this());
}

}
