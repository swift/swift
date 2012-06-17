/*
 * Copyright (c) 2012 Kevin Smith
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#include <Swiften/Network/CachingNameOnlyDomainNameResolver.h>

#include <boost/smart_ptr/make_shared.hpp>

namespace Swift {

CachingNameOnlyDomainNameResolver::CachingNameOnlyDomainNameResolver(DomainNameResolver* realResolver, EventLoop*) : realResolver(realResolver) {
}

CachingNameOnlyDomainNameResolver::~CachingNameOnlyDomainNameResolver() {

}

DomainNameServiceQuery::ref CachingNameOnlyDomainNameResolver::createServiceQuery(const std::string& name) {
	//FIXME: Cache
	return realResolver->createServiceQuery(name);
}

DomainNameAddressQuery::ref CachingNameOnlyDomainNameResolver::createAddressQuery(const std::string& name) {
	//FIXME: Cache
	return realResolver->createAddressQuery(name);
}

}
