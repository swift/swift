/*
 * Copyright (c) 2010 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#include <Swiften/LinkLocal/DNSSD/PlatformDNSSDQuerierFactory.h>

#if defined(HAVE_BONJOUR)
#include <Swiften/LinkLocal/DNSSD/Bonjour/BonjourQuerier.h>
#elif defined(HAVE_AVAHI)
#include <Swiften/LinkLocal/DNSSD/Avahi/AvahiQuerier.h>
#endif


namespace Swift {

PlatformDNSSDQuerierFactory::PlatformDNSSDQuerierFactory(EventLoop* eventLoop) : eventLoop(eventLoop) {
}

boost::shared_ptr<DNSSDQuerier> PlatformDNSSDQuerierFactory::createQuerier() {
#if defined(HAVE_BONJOUR)
	return boost::shared_ptr<DNSSDQuerier>(new BonjourQuerier(eventLoop));
#elif defined(HAVE_AVAHI)
	return boost::shared_ptr<DNSSDQuerier>(new AvahiQuerier(eventLoop));
#else
	return boost::shared_ptr<DNSSDQuerier>();
#endif
}

bool PlatformDNSSDQuerierFactory::canCreate() {
#if defined(HAVE_BONJOUR) || defined(HAVE_AVAHI)
	return true;
#else
	return false;
#endif
}

}
