/*
 * Copyright (c) 2010 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

// FIXME: This example is not complete yet

#include <iostream>

#include <Swiften/EventLoop/SimpleEventLoop.h>
#include <Swiften/LinkLocal/DNSSD/PlatformDNSSDQuerierFactory.h>
#include <Swiften/LinkLocal/DNSSD/DNSSDQuerier.h>
#include <Swiften/LinkLocal/DNSSD/DNSSDBrowseQuery.h>
#include <Swiften/LinkLocal/DNSSD/DNSSDResolveServiceQuery.h>

using namespace Swift;

int main(int argc, char* argv[]) {
	if (argc < 2) {
		std::cerr << "Invalid parameters" << std::endl;
		return -1;
	}

	SimpleEventLoop eventLoop;
	PlatformDNSSDQuerierFactory factory(&eventLoop);
	boost::shared_ptr<DNSSDQuerier> querier = factory.createQuerier();
	querier->start();

	if (std::string(argv[1]) == "browse") {
		boost::shared_ptr<DNSSDBrowseQuery> browseQuery = querier->createBrowseQuery();
		browseQuery->startBrowsing();
		eventLoop.run();
		browseQuery->stopBrowsing();
	}
	else if (std::string(argv[1]) == "resolve-service") {
		if (argc < 5) {
			std::cerr << "Invalid parameters" << std::endl;
			return -1;
		}
		boost::shared_ptr<DNSSDResolveServiceQuery> resolveQuery = querier->createResolveServiceQuery(DNSSDServiceID(argv[2], argv[3], argv[4]));
		resolveQuery->start();
		eventLoop.run();
		std::cerr << "Done running" << std::endl;
		resolveQuery->stop();
	}

	querier->stop();
}
