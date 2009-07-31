#pragma once

#include "Swiften/Base/String.h"
#include "Swiften/LinkLocal/DNSSD/Fake/FakeDNSSDQuery.h"
#include "Swiften/LinkLocal/DNSSD/DNSSDResolveHostnameQuery.h"
#include "Swiften/Network/HostAddress.h"

#include <netinet/in.h>

namespace Swift {
	class FakeDNSSDQuerier;

	class FakeDNSSDResolveHostnameQuery : public DNSSDResolveHostnameQuery, public FakeDNSSDQuery {
		public:	
			FakeDNSSDResolveHostnameQuery(const String& hostname, int interfaceIndex, boost::shared_ptr<FakeDNSSDQuerier> querier) : FakeDNSSDQuery(querier), hostname(hostname), interfaceIndex(interfaceIndex) {
			}

			void run() {
				FakeDNSSDQuery::run();
			}

			void finish() {
				FakeDNSSDQuery::finish();
			}

			String hostname;
			int interfaceIndex;
	};
}
