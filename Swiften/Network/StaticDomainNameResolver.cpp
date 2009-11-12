#include "Swiften/Network/StaticDomainNameResolver.h"
#include "Swiften/Network/DomainNameResolveException.h"
#include "Swiften/Base/String.h"

namespace Swift {

StaticDomainNameResolver::StaticDomainNameResolver() {
}

std::vector<HostAddressPort> StaticDomainNameResolver::resolve(const String& queriedDomain) {
	std::vector<HostAddressPort> result;

	for(DomainCollection::const_iterator i = domains.begin(); i != domains.end(); ++i) {
		if (i->first == queriedDomain) {
			result.push_back(i->second);
		}
	}
	if (result.empty()) {
		throw DomainNameResolveException();
	}
	return result;
}

void StaticDomainNameResolver::addDomain(const String& domain, const HostAddressPort& addressPort) {
	domains.push_back(std::make_pair(domain, addressPort));
}

}
