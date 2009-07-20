#include <string>
#include <boost/bind.hpp>

#include "Swiften/Base/Platform.h"
#if defined(SWIFTEN_PLATFORM_MACOSX) || defined(SWIFTEN_PLATFORM_WINDOWS)
#include "Swiften/LinkLocal/AppleDNSSDService.h"
#else
#include "Swiften/LinkLocal/AvahiDNSSDService.h"
#endif
#include "Slimber/Server.h"
#include "Swiften/EventLoop/SimpleEventLoop.h"

using namespace Swift;

int main() {
	SimpleEventLoop eventLoop;

	boost::shared_ptr<DNSSDService> dnsSDService;
#if defined(SWIFTEN_PLATFORM_MACOSX) || defined(SWIFTEN_PLATFORM_WINDOWS)
	dnsSDService = boost::shared_ptr<AppleDNSSDService>(
			new AppleDNSSDService());
#else
	dnsSDService = boost::shared_ptr<AvahiDNSSDService>(
			new AvahiDNSSDService());
#endif

	Server server(5222, 5562, dnsSDService);
	eventLoop.run();
	return 0;
}
