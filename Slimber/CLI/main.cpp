#include <string>
#include <boost/bind.hpp>

#include "Swiften/Base/Platform.h"
#if defined(SWIFTEN_PLATFORM_MACOSX) || defined(SWIFTEN_PLATFORM_WINDOWS)
#include "Swiften/LinkLocal/AppleDNSSDService.h"
#else
#include "Swiften/LinkLocal/AvahiDNSSDService.h"
#endif
#include "Slimber/Server.h"
#include "Slimber/FileVCardCollection.h"
#include "Swiften/LinkLocal/LinkLocalRoster.h"
#include "Swiften/LinkLocal/BonjourQuerier.h"
#include "Swiften/EventLoop/SimpleEventLoop.h"
#include "Swiften/Application/Platform/PlatformApplication.h"

using namespace Swift;

int main() {
	SimpleEventLoop eventLoop;
	boost::shared_ptr<BonjourQuerier> querier(new BonjourQuerier());
	querier->start();
	boost::shared_ptr<DNSSDBrowseQuery> query = querier->createBrowseQuery();
	query->startBrowsing();
	boost::shared_ptr<DNSSDRegisterQuery> query2 = querier->createRegisterQuery("remko", 1234, LinkLocalServiceInfo());
	query2->registerService();

/*
	boost::shared_ptr<DNSSDService> dnsSDService;
#if defined(SWIFTEN_PLATFORM_MACOSX) || defined(SWIFTEN_PLATFORM_WINDOWS)
	dnsSDService = boost::shared_ptr<AppleDNSSDService>(
			new AppleDNSSDService());
#else
	dnsSDService = boost::shared_ptr<AvahiDNSSDService>(
			new AvahiDNSSDService());
#endif

	boost::shared_ptr<LinkLocalRoster> linkLocalRoster = boost::shared_ptr<LinkLocalRoster>(new LinkLocalRoster(dnsSDService));

	FileVCardCollection vCardCollection(PlatformApplication("Slimber").getSettingsDir());

	Server server(5222, 5562, linkLocalRoster, dnsSDService, &vCardCollection);
	*/
	
	eventLoop.run();
	return 0;
}
