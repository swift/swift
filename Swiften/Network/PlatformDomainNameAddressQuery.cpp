/*
 * Copyright (c) 2010-2015 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#include <Swiften/Network/PlatformDomainNameAddressQuery.h>

#include <boost/asio/ip/tcp.hpp>

#include <Swiften/EventLoop/EventLoop.h>
#include <Swiften/Network/PlatformDomainNameResolver.h>

namespace Swift {

PlatformDomainNameAddressQuery::PlatformDomainNameAddressQuery(const boost::optional<std::string>& host, EventLoop* eventLoop, PlatformDomainNameResolver* resolver) : PlatformDomainNameQuery(resolver), hostnameValid(false), eventLoop(eventLoop) {
    if (!!host) {
        hostname = *host;
        hostnameValid = true;
    }
}

PlatformDomainNameAddressQuery::~PlatformDomainNameAddressQuery() {

}

void PlatformDomainNameAddressQuery::run() {
    getResolver()->addQueryToQueue(shared_from_this());
}

void PlatformDomainNameAddressQuery::runBlocking() {
    if (!hostnameValid) {
        emitError();
        return;
    }
    //std::cout << "PlatformDomainNameResolver::doRun()" << std::endl;
    boost::asio::ip::tcp::resolver resolver(ioService);
    boost::asio::ip::tcp::resolver::query query(hostname, "5222", boost::asio::ip::resolver_query_base::passive);
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
