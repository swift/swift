#pragma once

#include "Swiften/Base/String.h"
#include "Swiften/Network/HostAddress.h"
#include "Swiften/LinkLocal/LinkLocalServiceID.h"
#include "Swiften/LinkLocal/LinkLocalServiceInfo.h"

namespace Swift {
	class LinkLocalService {
		public:
			LinkLocalService(
					const LinkLocalServiceID& id,
					const DNSSDService::ResolveResult& info) :
						id(id),
						info(info) {}

			const LinkLocalServiceID& getID() const {
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
			LinkLocalServiceID id;
			DNSSDService::ResolveResult info;
	};
}
