/*
 * Copyright (c) 2012-2016 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#include <Swiften/Network/CachingDomainNameResolver.h>

#include <memory>

namespace Swift {

CachingDomainNameResolver::CachingDomainNameResolver(DomainNameResolver* realResolver, EventLoop*) : realResolver(realResolver) {
}

CachingDomainNameResolver::~CachingDomainNameResolver() {

}

DomainNameServiceQuery::ref CachingDomainNameResolver::createServiceQuery(const std::string& serviceLookupPrefix, const std::string& domain) {
    //TODO: Cache
    return realResolver->createServiceQuery(serviceLookupPrefix, domain);
}

DomainNameAddressQuery::ref CachingDomainNameResolver::createAddressQuery(const std::string& name) {
    //TODO: Cache
    return realResolver->createAddressQuery(name);
}

}
