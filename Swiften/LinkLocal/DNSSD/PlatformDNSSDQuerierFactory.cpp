#include "Swiften/LinkLocal/DNSSD/PlatformDNSSDQuerierFactory.h"

#ifdef HAVE_SWIFTEN_CONFIG_H
#include "Swiften/config.h"
#endif
#if defined(HAVE_BONJOUR)
#include "Swiften/LinkLocal/DNSSD/Bonjour/BonjourQuerier.h"
#elif defined(HAVE_AVAHI)
#include "Swiften/LinkLocal/DNSSD/Avahi/AvahiQuerier.h"
#endif


namespace Swift {

boost::shared_ptr<DNSSDQuerier> PlatformDNSSDQuerierFactory::createQuerier() {
#if defined(HAVE_BONJOUR)
	return boost::shared_ptr<DNSSDQuerier>(new BonjourQuerier());
#elif defined(HAVE_AVAHI)
	return boost::shared_ptr<DNSSDQuerier>(new AvahiQuerier());
#else
	return boost::shared_ptr<DNSSDQuerier>();
#endif
}

}
