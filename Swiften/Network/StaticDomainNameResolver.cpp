/*
 * Copyright (c) 2010-2018 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#include <Swiften/Network/StaticDomainNameResolver.h>

#include <string>

#include <boost/bind.hpp>
#include <boost/lexical_cast.hpp>

#include <Swiften/EventLoop/EventOwner.h>
#include <Swiften/Network/DomainNameResolveError.h>

using namespace Swift;

namespace {
    struct ServiceQuery : public DomainNameServiceQuery, public std::enable_shared_from_this<ServiceQuery> {
        ServiceQuery(const std::string& service, Swift::StaticDomainNameResolver* resolver, EventLoop* eventLoop, std::shared_ptr<EventOwner> owner) : eventLoop(eventLoop), service(service), resolver(resolver), owner(owner) {}

        virtual void run() {
            if (!resolver->getIsResponsive()) {
                return;
            }
            std::vector<DomainNameServiceQuery::Result> results;
            for(const auto& i : resolver->getServices()) {
                if (i.first == service) {
                    results.push_back(i.second);
                }
            }
            eventLoop->postEvent(boost::bind(&ServiceQuery::emitOnResult, shared_from_this(), results), owner);
        }

        void emitOnResult(std::vector<DomainNameServiceQuery::Result> results) {
            onResult(results);
        }

        EventLoop* eventLoop;
        std::string service;
        StaticDomainNameResolver* resolver;
        std::shared_ptr<EventOwner> owner;
    };

    struct AddressQuery : public DomainNameAddressQuery, public std::enable_shared_from_this<AddressQuery> {
        AddressQuery(const std::string& host, StaticDomainNameResolver* resolver, EventLoop* eventLoop, std::shared_ptr<EventOwner> owner) : eventLoop(eventLoop), host(host), resolver(resolver), owner(owner) {}

        virtual void run() {
            if (!resolver->getIsResponsive()) {
                return;
            }
            if (auto address = HostAddress::fromString(host)) {
                // IP Literals should resolve to themselves
                resolver->addAddress(host, *address);
            }
            StaticDomainNameResolver::AddressesMap::const_iterator i = resolver->getAddresses().find(host);
            if (i != resolver->getAddresses().end()) {
                eventLoop->postEvent(
                        boost::bind(&AddressQuery::emitOnResult, shared_from_this(), i->second, boost::optional<DomainNameResolveError>()));
            }
            else {
                eventLoop->postEvent(boost::bind(&AddressQuery::emitOnResult, shared_from_this(), std::vector<HostAddress>(), boost::optional<DomainNameResolveError>(DomainNameResolveError())), owner);
            }
        }

        void emitOnResult(std::vector<HostAddress> results, boost::optional<DomainNameResolveError> error) {
            onResult(results, error);
        }

        EventLoop* eventLoop;
        std::string host;
        StaticDomainNameResolver* resolver;
        std::shared_ptr<EventOwner> owner;
    };
}

class StaticDomainNameResolverEventOwner : public EventOwner {
    public:
        ~StaticDomainNameResolverEventOwner() {

        }
};


namespace Swift {

StaticDomainNameResolver::StaticDomainNameResolver(EventLoop* eventLoop) : eventLoop(eventLoop), isResponsive(true), owner(new StaticDomainNameResolverEventOwner()) {
}

StaticDomainNameResolver::~StaticDomainNameResolver() {
    eventLoop->removeEventsFromOwner(owner);
}

void StaticDomainNameResolver::addAddress(const std::string& domain, const HostAddress& address) {
    addresses[domain].push_back(address);
}

void StaticDomainNameResolver::addService(const std::string& service, const DomainNameServiceQuery::Result& result) {
    services.push_back(std::make_pair(service, result));
}

void StaticDomainNameResolver::addXMPPClientService(const std::string& domain, const HostAddressPort& address) {
    static int hostid = 0;
    std::string hostname(std::string("host-") + std::to_string(hostid));
    hostid++;

    addService("_xmpp-client._tcp." + domain, ServiceQuery::Result(hostname, address.getPort(), 0, 0));
    addAddress(hostname, address.getAddress());
}

void StaticDomainNameResolver::addXMPPClientService(const std::string& domain, const std::string& hostname, unsigned short port) {
    addService("_xmpp-client._tcp." + domain, ServiceQuery::Result(hostname, port, 0, 0));
}

std::shared_ptr<DomainNameServiceQuery> StaticDomainNameResolver::createServiceQuery(const std::string& serviceLookupPrefix, const std::string& domain) {
    return std::make_shared<ServiceQuery>(serviceLookupPrefix + domain, this, eventLoop, owner);
}

std::shared_ptr<DomainNameAddressQuery> StaticDomainNameResolver::createAddressQuery(const std::string& name) {
    return std::make_shared<AddressQuery>(name, this, eventLoop, owner);
}

}
