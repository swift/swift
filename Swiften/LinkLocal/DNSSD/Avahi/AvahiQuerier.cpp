/*
 * Copyright (c) 2010-2018 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#include <Swiften/LinkLocal/DNSSD/Avahi/AvahiQuerier.h>

#include <iostream>

#include <Swiften/LinkLocal/DNSSD/Avahi/AvahiBrowseQuery.h>
#include <Swiften/LinkLocal/DNSSD/Avahi/AvahiRegisterQuery.h>
#include <Swiften/LinkLocal/DNSSD/Avahi/AvahiResolveHostnameQuery.h>
#include <Swiften/LinkLocal/DNSSD/Avahi/AvahiResolveServiceQuery.h>

namespace Swift {

AvahiQuerier::AvahiQuerier(EventLoop* eventLoop) : eventLoop(eventLoop), client(NULL), threadedPoll(NULL) {
}

AvahiQuerier::~AvahiQuerier() {
}

std::shared_ptr<DNSSDBrowseQuery> AvahiQuerier::createBrowseQuery() {
    return std::make_shared<AvahiBrowseQuery>(shared_from_this(), eventLoop);
}

std::shared_ptr<DNSSDRegisterQuery> AvahiQuerier::createRegisterQuery(const std::string& name, unsigned short port, const ByteArray& info) {
    return std::make_shared<AvahiRegisterQuery>(name, port, info, shared_from_this(), eventLoop);
}

std::shared_ptr<DNSSDResolveServiceQuery> AvahiQuerier::createResolveServiceQuery(const DNSSDServiceID& service) {
    return std::make_shared<AvahiResolveServiceQuery>(service, shared_from_this(), eventLoop);
}

std::shared_ptr<DNSSDResolveHostnameQuery> AvahiQuerier::createResolveHostnameQuery(const std::string& hostname, int interfaceIndex) {
    return std::make_shared<AvahiResolveHostnameQuery>(hostname, interfaceIndex, shared_from_this(), eventLoop);
}

void AvahiQuerier::start() {
    std::cout << "Starrting querier" << std::endl;
    assert(!threadedPoll);
    threadedPoll = avahi_threaded_poll_new();
    int error;
    assert(!client);
    client = avahi_client_new(
            avahi_threaded_poll_get(threadedPoll),
            static_cast<AvahiClientFlags>(0), NULL, this, &error); // TODO
    if (!client) {
        // TODO
        std::cerr << "Avahi Error: " << avahi_strerror(error) << std::endl;
        return;
    }
    std::cout << "Starrting event loop" << std::endl;
    avahi_threaded_poll_start(threadedPoll);
}

void AvahiQuerier::stop() {
    assert(threadedPoll);
    avahi_threaded_poll_stop(threadedPoll);
    assert(client);
    avahi_client_free(client);
    avahi_threaded_poll_free(threadedPoll);
}

}
