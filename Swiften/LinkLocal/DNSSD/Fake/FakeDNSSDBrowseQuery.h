#pragma once

#include "Swiften/LinkLocal/DNSSD/Fake/FakeDNSSDQuery.h"
#include "Swiften/LinkLocal/DNSSD/DNSSDBrowseQuery.h"

namespace Swift {
	class FakeDNSSDQuerier;

	class FakeDNSSDBrowseQuery : public DNSSDBrowseQuery, public FakeDNSSDQuery {
		public:	
			FakeDNSSDBrowseQuery(boost::shared_ptr<FakeDNSSDQuerier> querier) : FakeDNSSDQuery(querier) {
			}

			void startBrowsing() {
				run();
			}

			void stopBrowsing() {
				finish();
			}
	};
}
