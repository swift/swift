#ifndef SWIFTEN_DOMAINNAMERESOLVER_H
#define SWIFTEN_DOMAINNAMERESOLVER_H

#include <string>

#include "Swiften/Base/String.h"
#include "Swiften/Network/HostAddress.h"
#include "Swiften/Network/HostAddressPort.h"

namespace Swift {
	class String;

	class DomainNameResolver {
		public:
			DomainNameResolver();
			virtual ~DomainNameResolver();

			HostAddressPort resolve(const String& domain);

		private:
			virtual HostAddressPort resolveDomain(const std::string& domain);
			HostAddressPort resolveXMPPService(const std::string& domain);
			HostAddress resolveHostName(const std::string& hostName);
	};
}

#endif
