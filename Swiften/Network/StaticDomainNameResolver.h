#pragma once

#include <vector>
#include <map>

#include "Swiften/Network/DomainNameResolver.h"

namespace Swift {
	class String;

	class StaticDomainNameResolver : public DomainNameResolver {
		public:
			StaticDomainNameResolver();

			virtual std::vector<HostAddressPort> resolve(const String& domain);

			void addDomain(const String& domain, const HostAddressPort& addressPort);
			
		private:
			typedef std::vector< std::pair<String, HostAddressPort> > DomainCollection;
			DomainCollection domains;
	};
}
