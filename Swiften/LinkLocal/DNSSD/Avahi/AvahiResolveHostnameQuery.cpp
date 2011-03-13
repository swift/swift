/*
 * Copyright (c) 2010 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#include <Swiften/LinkLocal/DNSSD/Avahi/AvahiResolveHostnameQuery.h>

#include <iostream>
#include <boost/bind.hpp>

namespace Swift {

AvahiResolveHostnameQuery::AvahiResolveHostnameQuery(const std::string& hostname, int, boost::shared_ptr<AvahiQuerier> querier, EventLoop* eventLoop) : AvahiQuery(querier, eventLoop), hostname(hostname) {
	std::cout << "Resolving hostname " << hostname << std::endl;
}

void AvahiResolveHostnameQuery::run() {
		eventLoop->postEvent(boost::bind(boost::ref(onHostnameResolved), boost::optional<HostAddress>(HostAddress(hostname))), shared_from_this());
}

}
