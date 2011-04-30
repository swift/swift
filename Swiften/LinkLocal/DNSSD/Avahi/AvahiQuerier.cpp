/*
 * Copyright (c) 2010 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#include <Swiften/LinkLocal/DNSSD/Avahi/AvahiQuerier.h>

#include <iostream>

#include <Swiften/LinkLocal/DNSSD/Avahi/AvahiBrowseQuery.h>
#include <Swiften/LinkLocal/DNSSD/Avahi/AvahiResolveServiceQuery.h>
#include <Swiften/LinkLocal/DNSSD/Avahi/AvahiResolveHostnameQuery.h>
#include <Swiften/LinkLocal/DNSSD/Avahi/AvahiRegisterQuery.h>

namespace Swift {

AvahiQuerier::AvahiQuerier(EventLoop* eventLoop) : eventLoop(eventLoop), client(NULL), threadedPoll(NULL) {
}

AvahiQuerier::~AvahiQuerier() {
}

boost::shared_ptr<DNSSDBrowseQuery> AvahiQuerier::createBrowseQuery() {
	return boost::shared_ptr<DNSSDBrowseQuery>(new AvahiBrowseQuery(shared_from_this(), eventLoop));
}

boost::shared_ptr<DNSSDRegisterQuery> AvahiQuerier::createRegisterQuery(const std::string& name, int port, const ByteArray& info) {
	return boost::shared_ptr<DNSSDRegisterQuery>(new AvahiRegisterQuery(name, port, info, shared_from_this(), eventLoop));
}

boost::shared_ptr<DNSSDResolveServiceQuery> AvahiQuerier::createResolveServiceQuery(const DNSSDServiceID& service) {
	return boost::shared_ptr<DNSSDResolveServiceQuery>(new AvahiResolveServiceQuery(service, shared_from_this(), eventLoop));
}

boost::shared_ptr<DNSSDResolveHostnameQuery> AvahiQuerier::createResolveHostnameQuery(const std::string& hostname, int interfaceIndex) {
	return boost::shared_ptr<DNSSDResolveHostnameQuery>(new AvahiResolveHostnameQuery(hostname, interfaceIndex, shared_from_this(), eventLoop));
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
