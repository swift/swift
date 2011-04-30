/*
 * Copyright (c) 2010 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#include <Swiften/LinkLocal/DNSSD/Fake/FakeDNSSDQuery.h>
#include <Swiften/LinkLocal/DNSSD/Fake/FakeDNSSDQuerier.h>

namespace Swift {

FakeDNSSDQuery::FakeDNSSDQuery(boost::shared_ptr<FakeDNSSDQuerier> querier) : querier(querier) {
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
