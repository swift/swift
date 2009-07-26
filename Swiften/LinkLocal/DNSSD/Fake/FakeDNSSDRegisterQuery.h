#pragma once

#include "Swiften/LinkLocal/DNSSD/Fake/FakeDNSSDQuery.h"
#include "Swiften/LinkLocal/DNSSD/DNSSDRegisterQuery.h"
#include "Swiften/LinkLocal/LinkLocalServiceInfo.h"
#include "Swiften/Base/String.h"

namespace Swift {
	class FakeDNSSDQuerier;

	class FakeDNSSDRegisterQuery : public DNSSDRegisterQuery, public FakeDNSSDQuery {
		public:	
			FakeDNSSDRegisterQuery(const String& name, int port, const LinkLocalServiceInfo& info, boost::shared_ptr<FakeDNSSDQuerier> querier) : FakeDNSSDQuery(querier), name(name), port(port), info(info) {
			}

			void registerService() {
				run();
			}

			void updateServiceInfo(const LinkLocalServiceInfo&) {
			}

			void unregisterService() {
				finish();
			}

			String name;
			int port;
			LinkLocalServiceInfo info;
	};
}
