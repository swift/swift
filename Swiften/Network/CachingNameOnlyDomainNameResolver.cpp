/*
 * Copyright (c) 2012 Kevin Smith
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#include <Swiften/Network/CachingNameOnlyDomainNameResolver.h>

#include <boost/smart_ptr/make_shared.hpp>

namespace Swift {
CachingNameOnlyDomainNameResolver::CachingNameOnlyDomainNameResolver(DomainNameResolver* realResolver, EventLoop* eventLoop) : realResolver(realResolver) {
	staticResolver = boost::make_shared<StaticDomainNameResolver>(eventLoop);
}

CachingNameOnlyDomainNameResolver::~CachingNameOnlyDomainNameResolver() {

}

DomainNameServiceQuery::ref CachingNameOnlyDomainNameResolver::createServiceQuery(const std::string& name) {
	return staticResolver->createServiceQuery(name);
}

DomainNameAddressQuery::ref CachingNameOnlyDomainNameResolver::createAddressQuery(const std::string& name) {
	return realResolver->createAddressQuery(name);
}

void CachingNameOnlyDomainNameResolver::handleAddressQueryResult(const std::string hostname, const std::vector<HostAddress>& address, boost::optional<DomainNameResolveError> error) {
	//FIXME: Cache
}

}
