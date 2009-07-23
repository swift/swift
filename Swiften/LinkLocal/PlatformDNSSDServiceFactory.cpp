#include "Swiften/LinkLocal/PlatformDNSSDServiceFactory.h"

#include <cassert>

#ifdef HAVE_SWIFTEN_CONFIG_H
#include "Swiften/config.h"
#endif
#if defined(HAVE_AVAHI)
#include "Swiften/LinkLocal/AvahiDNSSDService.h"
#elif defined(HAVE_BONJOUR)
#include "Swiften/LinkLocal/AppleDNSSDService.h"
#endif

namespace Swift {

PlatformDNSSDServiceFactory::PlatformDNSSDServiceFactory() {
}

bool PlatformDNSSDServiceFactory::canCreate() const {
#if defined(HAVE_AVAHI) || defined(HAVE_BONJOUR)
	return true;
#else
	return false;
#endif
}

boost::shared_ptr<DNSSDService> PlatformDNSSDServiceFactory::createDNSSDService() {
#if defined(HAVE_AVAHI)
	return boost::shared_ptr<DNSSDService>(new AvahiDNSSDService());
#elif defined(HAVE_BONJOUR)
  return boost::shared_ptr<DNSSDService>(new AppleDNSSDService());
#else
	assert(false);
	return boost::shared_ptr<DNSSDService>();
#endif
}

}
