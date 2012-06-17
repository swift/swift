/*
 * Copyright (c) 2012 Kevin Smith
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#include <Swiften/Network/CachingDomainNameResolver.h>

#include <boost/smart_ptr/make_shared.hpp>

namespace Swift {

CachingDomainNameResolver::CachingDomainNameResolver(DomainNameResolver* realResolver, EventLoop*) : realResolver(realResolver) {
}

CachingDomainNameResolver::~CachingDomainNameResolver() {

}

DomainNameServiceQuery::ref CachingDomainNameResolver::createServiceQuery(const std::string& name) {
	//TODO: Cache
	return realResolver->createServiceQuery(name);
}

DomainNameAddressQuery::ref CachingDomainNameResolver::createAddressQuery(const std::string& name) {
	//TODO: Cache
	return realResolver->createAddressQuery(name);
}

}
