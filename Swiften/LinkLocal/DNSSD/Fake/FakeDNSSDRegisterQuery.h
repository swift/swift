/*
 * Copyright (c) 2010 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#pragma once

#include <Swiften/LinkLocal/DNSSD/Fake/FakeDNSSDQuery.h>
#include <Swiften/LinkLocal/DNSSD/DNSSDRegisterQuery.h>
#include <Swiften/Base/ByteArray.h>
#include <string>

namespace Swift {
	class FakeDNSSDQuerier;

	class FakeDNSSDRegisterQuery : public DNSSDRegisterQuery, public FakeDNSSDQuery {
		public:	
			FakeDNSSDRegisterQuery(const std::string& name, int port, const ByteArray& info, boost::shared_ptr<FakeDNSSDQuerier> querier) : FakeDNSSDQuery(querier), name(name), port(port), info(info) {
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

			std::string name;
			int port;
			ByteArray info;
	};
}
