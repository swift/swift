/*
 * Copyright (c) 2010-2016 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#include <Swiften/LinkLocal/DNSSD/Fake/FakeDNSSDQuerier.h>

#include <iostream>

#include <boost/bind.hpp>

#include <Swiften/Base/Algorithm.h>
#include <Swiften/EventLoop/EventLoop.h>
#include <Swiften/LinkLocal/DNSSD/Fake/FakeDNSSDBrowseQuery.h>
#include <Swiften/LinkLocal/DNSSD/Fake/FakeDNSSDRegisterQuery.h>
#include <Swiften/LinkLocal/DNSSD/Fake/FakeDNSSDResolveHostnameQuery.h>
#include <Swiften/LinkLocal/DNSSD/Fake/FakeDNSSDResolveServiceQuery.h>

namespace Swift {

FakeDNSSDQuerier::FakeDNSSDQuerier(const std::string& domain, EventLoop* eventLoop) : domain(domain), eventLoop(eventLoop) {
}

FakeDNSSDQuerier::~FakeDNSSDQuerier() {
    if (!runningQueries.empty()) {
        std::cerr << "FakeDNSSDQuerier: Running queries not empty at destruction time" << std::endl;
    }
}

std::shared_ptr<DNSSDBrowseQuery> FakeDNSSDQuerier::createBrowseQuery() {
    return std::make_shared<FakeDNSSDBrowseQuery>(shared_from_this());
}

std::shared_ptr<DNSSDRegisterQuery> FakeDNSSDQuerier::createRegisterQuery(const std::string& name, int port, const ByteArray& info) {
    return std::make_shared<FakeDNSSDRegisterQuery>(name, port, info, shared_from_this());
}

std::shared_ptr<DNSSDResolveServiceQuery> FakeDNSSDQuerier::createResolveServiceQuery(const DNSSDServiceID& service) {
    return std::make_shared<FakeDNSSDResolveServiceQuery>(service, shared_from_this());
}

std::shared_ptr<DNSSDResolveHostnameQuery> FakeDNSSDQuerier::createResolveHostnameQuery(const std::string& hostname, int interfaceIndex) {
    return std::make_shared<FakeDNSSDResolveHostnameQuery>(hostname, interfaceIndex, shared_from_this());
}

void FakeDNSSDQuerier::addRunningQuery(std::shared_ptr<FakeDNSSDQuery> query) {
    runningQueries.push_back(query);
    allQueriesEverRun.push_back(query);
    if (std::shared_ptr<FakeDNSSDBrowseQuery> browseQuery = std::dynamic_pointer_cast<FakeDNSSDBrowseQuery>(query)) {
        for (const auto& service : services) {
            eventLoop->postEvent(boost::bind(boost::ref(browseQuery->onServiceAdded), service), shared_from_this());
        }
    }
    else if (std::shared_ptr<FakeDNSSDResolveServiceQuery> resolveQuery = std::dynamic_pointer_cast<FakeDNSSDResolveServiceQuery>(query)) {
        for (const auto& i : serviceInfo) {
            if (i.first == resolveQuery->service) {
                eventLoop->postEvent(boost::bind(boost::ref(resolveQuery->onServiceResolved), i.second), shared_from_this());
            }
        }
    }
    else if (std::shared_ptr<FakeDNSSDRegisterQuery> registerQuery = std::dynamic_pointer_cast<FakeDNSSDRegisterQuery>(query)) {
        DNSSDServiceID service(registerQuery->name, domain);
        eventLoop->postEvent(boost::bind(boost::ref(registerQuery->onRegisterFinished), service), shared_from_this());
    }
    else if (std::shared_ptr<FakeDNSSDResolveHostnameQuery> resolveHostnameQuery = std::dynamic_pointer_cast<FakeDNSSDResolveHostnameQuery>(query)) {
        std::map<std::string,boost::optional<HostAddress> >::const_iterator i = addresses.find(resolveHostnameQuery->hostname);
        if (i != addresses.end()) {
            eventLoop->postEvent(
                    boost::bind(
                        boost::ref(resolveHostnameQuery->onHostnameResolved), i->second),
                    shared_from_this());
        }
    }
}

void FakeDNSSDQuerier::removeRunningQuery(std::shared_ptr<FakeDNSSDQuery> query) {
    erase(runningQueries, query);
}

void FakeDNSSDQuerier::addService(const DNSSDServiceID& id) {
    services.insert(id);
    for (const auto& query : getQueries<FakeDNSSDBrowseQuery>()) {
        eventLoop->postEvent(boost::bind(boost::ref(query->onServiceAdded), id), shared_from_this());
    }
}

void FakeDNSSDQuerier::removeService(const DNSSDServiceID& id) {
    services.erase(id);
    serviceInfo.erase(id);
    for (const auto& query : getQueries<FakeDNSSDBrowseQuery>()) {
        eventLoop->postEvent(boost::bind(boost::ref(query->onServiceRemoved), id), shared_from_this());
    }
}

void FakeDNSSDQuerier::setServiceInfo(const DNSSDServiceID& id, const DNSSDResolveServiceQuery::Result& info) {
    std::pair<ServiceInfoMap::iterator, bool> r = serviceInfo.insert(std::make_pair(id, info));
    if (!r.second) {
        r.first->second = info;
    }
    for (const auto& query : getQueries<FakeDNSSDResolveServiceQuery>()) {
        if (query->service == id) {
            eventLoop->postEvent(boost::bind(boost::ref(query->onServiceResolved), info), shared_from_this());
        }
    }
}

bool FakeDNSSDQuerier::isServiceRegistered(const std::string& name, int port, const ByteArray& info) {
    for (const auto& query : getQueries<FakeDNSSDRegisterQuery>()) {
        if (query->name == name && query->port == port && query->info == info) {
            return true;
        }
    }
    return false;
}

void FakeDNSSDQuerier::setBrowseError() {
    for (const auto& query : getQueries<FakeDNSSDBrowseQuery>()) {
        eventLoop->postEvent(boost::ref(query->onError), shared_from_this());
    }
}

void FakeDNSSDQuerier::setRegisterError() {
    for (const auto& query : getQueries<FakeDNSSDRegisterQuery>()) {
        eventLoop->postEvent(boost::bind(boost::ref(query->onRegisterFinished), boost::optional<DNSSDServiceID>()), shared_from_this());
    }
}

void FakeDNSSDQuerier::setAddress(const std::string& hostname, boost::optional<HostAddress> address) {
    addresses[hostname] = address;
    for (const auto& query : getQueries<FakeDNSSDResolveHostnameQuery>()) {
        if (query->hostname == hostname) {
            eventLoop->postEvent(boost::bind(
                    boost::ref(query->onHostnameResolved), address), shared_from_this());
        }
    }
}

}
