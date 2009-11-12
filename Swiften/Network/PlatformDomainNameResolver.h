#pragma once

#include <string>
#include <vector>

#include "Swiften/Base/String.h"
#include "Swiften/Network/DomainNameResolver.h"
#include "Swiften/Network/HostAddress.h"
#include "Swiften/Network/HostAddressPort.h"

namespace Swift {
	class String;

	class PlatformDomainNameResolver : public DomainNameResolver {
		public:
			PlatformDomainNameResolver();

			std::vector<HostAddressPort> resolve(const String& domain);

		private:
			std::vector<HostAddressPort> resolveDomain(const std::string& domain);
			std::vector<HostAddressPort> resolveXMPPService(const std::string& domain);
			HostAddress resolveHostName(const std::string& hostName);
	};
}
