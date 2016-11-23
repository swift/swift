/*
 * Copyright (c) 2010-2016 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#include <Swiften/LinkLocal/DNSSD/Avahi/AvahiResolveHostnameQuery.h>

#include <boost/bind.hpp>

#include <netinet/in.h>

#include <Swiften/Base/Log.h>

namespace Swift {

AvahiResolveHostnameQuery::AvahiResolveHostnameQuery(const std::string& hostname, int, std::shared_ptr<AvahiQuerier> querier, EventLoop* eventLoop) : AvahiQuery(querier, eventLoop), hostname(hostname) {
    SWIFT_LOG(debug) << "Resolving hostname " << hostname << std::endl;
}

void AvahiResolveHostnameQuery::run() {
        eventLoop->postEvent(boost::bind(boost::ref(onHostnameResolved), HostAddress::fromString(hostname)), shared_from_this());
}

}
