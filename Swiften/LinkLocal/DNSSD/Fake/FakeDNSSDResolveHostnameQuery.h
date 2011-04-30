/*
 * Copyright (c) 2010 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#pragma once

#include <string>
#include <Swiften/LinkLocal/DNSSD/Fake/FakeDNSSDQuery.h>
#include <Swiften/LinkLocal/DNSSD/DNSSDResolveHostnameQuery.h>
#include <Swiften/Network/HostAddress.h>

namespace Swift {
	class FakeDNSSDQuerier;

	class FakeDNSSDResolveHostnameQuery : public DNSSDResolveHostnameQuery, public FakeDNSSDQuery {
		public:	
			FakeDNSSDResolveHostnameQuery(const std::string& hostname, int interfaceIndex, boost::shared_ptr<FakeDNSSDQuerier> querier) : FakeDNSSDQuery(querier), hostname(hostname), interfaceIndex(interfaceIndex) {
			}

			void run() {
				FakeDNSSDQuery::run();
			}

			void finish() {
				FakeDNSSDQuery::finish();
			}

			std::string hostname;
			int interfaceIndex;
	};
}
