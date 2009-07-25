#pragma once

#include "Swiften/Base/String.h"
#include "Swiften/LinkLocal/DNSSD/DNSSDServiceID.h"
#include "Swiften/LinkLocal/DNSSD/DNSSDResolveServiceQuery.h"
#include "Swiften/LinkLocal/LinkLocalServiceInfo.h"

namespace Swift {
	class LinkLocalService {
		public:
			LinkLocalService(
					const DNSSDServiceID& id,
					const DNSSDResolveServiceQuery::Result& info) :
						id(id),
						info(info) {}

			const DNSSDServiceID& getID() const {
				return id;
			}

			const String& getName() const {
				return id.getName();
			}

			int getPort() const {
				return info.port;
			}

			const String& getHostname() const {
				return info.host;
			}

			const LinkLocalServiceInfo& getInfo() const {
				return info.info;
			}

		private:
			DNSSDServiceID id;
			DNSSDResolveServiceQuery::Result info;
	};
}
