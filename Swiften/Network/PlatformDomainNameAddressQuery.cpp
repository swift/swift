/*
 * Copyright (c) 2010 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#include <Swiften/Network/PlatformDomainNameAddressQuery.h>

#include <boost/asio/ip/tcp.hpp>

#include <Swiften/Network/PlatformDomainNameResolver.h>
#include <Swiften/EventLoop/EventLoop.h>

namespace Swift {

PlatformDomainNameAddressQuery::PlatformDomainNameAddressQuery(const std::string& host, EventLoop* eventLoop, PlatformDomainNameResolver* resolver) : PlatformDomainNameQuery(resolver), hostname(host), eventLoop(eventLoop) {
}

void PlatformDomainNameAddressQuery::run() {
	getResolver()->addQueryToQueue(shared_from_this());
}

void PlatformDomainNameAddressQuery::runBlocking() {
	//std::cout << "PlatformDomainNameResolver::doRun()" << std::endl;
	boost::asio::ip::tcp::resolver resolver(ioService);
	boost::asio::ip::tcp::resolver::query query(hostname, "5222");
	try {
		//std::cout << "PlatformDomainNameResolver::doRun(): Resolving" << std::endl;
		boost::asio::ip::tcp::resolver::iterator endpointIterator = resolver.resolve(query);
		//std::cout << "PlatformDomainNameResolver::doRun(): Resolved" << std::endl;
		if (endpointIterator == boost::asio::ip::tcp::resolver::iterator()) {
			//std::cout << "PlatformDomainNameResolver::doRun(): Error 1" << std::endl;
			emitError();
		}
		else {
			std::vector<HostAddress> results;
			for ( ; endpointIterator != boost::asio::ip::tcp::resolver::iterator(); ++endpointIterator) {
				boost::asio::ip::address address = (*endpointIterator).endpoint().address();
				results.push_back(address.is_v4() ? HostAddress(&address.to_v4().to_bytes()[0], 4) : HostAddress(&address.to_v6().to_bytes()[0], 16));
			}

			//std::cout << "PlatformDomainNameResolver::doRun(): Success" << std::endl;
			eventLoop->postEvent(
					boost::bind(boost::ref(onResult), results, boost::optional<DomainNameResolveError>()), 
					shared_from_this());
		}
	}
	catch (...) {
		//std::cout << "PlatformDomainNameResolver::doRun(): Error 2" << std::endl;
		emitError();
	}
}

void PlatformDomainNameAddressQuery::emitError() {
	eventLoop->postEvent(boost::bind(boost::ref(onResult), std::vector<HostAddress>(), boost::optional<DomainNameResolveError>(DomainNameResolveError())), shared_from_this());
}

}
