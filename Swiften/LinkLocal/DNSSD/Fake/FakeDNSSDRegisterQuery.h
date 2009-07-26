#pragma once

#include "Swiften/LinkLocal/DNSSD/Fake/FakeDNSSDQuery.h"
#include "Swiften/LinkLocal/DNSSD/DNSSDRegisterQuery.h"
#include "Swiften/Base/ByteArray.h"
#include "Swiften/Base/String.h"

namespace Swift {
	class FakeDNSSDQuerier;

	class FakeDNSSDRegisterQuery : public DNSSDRegisterQuery, public FakeDNSSDQuery {
		public:	
			FakeDNSSDRegisterQuery(const String& name, int port, const ByteArray& info, boost::shared_ptr<FakeDNSSDQuerier> querier) : FakeDNSSDQuery(querier), name(name), port(port), info(info) {
			}

			void registerService() {
				run();
			}

			void updateServiceInfo(const ByteArray& i) {
				info = i;
			}

			void unregisterService() {
				finish();
			}

			String name;
			int port;
			ByteArray info;
	};
}
