/*
 * Copyright (c) 2010 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#pragma once

#include <Swiften/LinkLocal/DNSSD/Fake/FakeDNSSDQuery.h>
#include <Swiften/LinkLocal/DNSSD/DNSSDResolveServiceQuery.h>
#include <Swiften/LinkLocal/LinkLocalServiceInfo.h>

namespace Swift {
	class FakeDNSSDQuerier;

	class FakeDNSSDResolveServiceQuery : public DNSSDResolveServiceQuery, public FakeDNSSDQuery {
		public:	
			FakeDNSSDResolveServiceQuery(const DNSSDServiceID& service, boost::shared_ptr<FakeDNSSDQuerier> querier) : FakeDNSSDQuery(querier), service(service) {
			}

			void start() {
				run();
			}

			void stop() {
				finish();
			}

			DNSSDServiceID service;
	};
}
