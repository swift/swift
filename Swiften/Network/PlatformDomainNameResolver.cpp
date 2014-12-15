/*
 * Copyright (c) 2010-2013 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#include <Swiften/Network/PlatformDomainNameResolver.h>

// Putting this early on, because some system types conflict with thread
#include <Swiften/Network/PlatformDomainNameServiceQuery.h>

#include <string>
#include <vector>
#include <boost/bind.hpp>
#include <boost/thread.hpp>
#include <algorithm>

#include <string>
#include <Swiften/IDN/IDNConverter.h>
#include <Swiften/Network/HostAddress.h>
#include <Swiften/EventLoop/EventLoop.h>
#include <Swiften/Network/HostAddressPort.h>
#include <Swiften/Network/DomainNameAddressQuery.h>
#include <Swiften/Network/PlatformDomainNameAddressQuery.h>

using namespace Swift;

namespace Swift {

PlatformDomainNameResolver::PlatformDomainNameResolver(IDNConverter* idnConverter, EventLoop* eventLoop) : idnConverter(idnConverter), eventLoop(eventLoop), stopRequested(false) {
	thread = new boost::thread(boost::bind(&PlatformDomainNameResolver::run, this));
}

PlatformDomainNameResolver::~PlatformDomainNameResolver() {
	stopRequested = true;
	addQueryToQueue(boost::shared_ptr<PlatformDomainNameQuery>());
	thread->join();
	delete thread;
}

boost::shared_ptr<DomainNameServiceQuery> PlatformDomainNameResolver::createServiceQuery(const std::string& serviceLookupPrefix, const std::string& domain) {
	boost::optional<std::string> encodedDomain = idnConverter->getIDNAEncoded(domain);
	std::string result;
	if (encodedDomain) {
		result = serviceLookupPrefix + *encodedDomain;
	}
	return boost::shared_ptr<DomainNameServiceQuery>(new PlatformDomainNameServiceQuery(result, eventLoop, this));
}

boost::shared_ptr<DomainNameAddressQuery> PlatformDomainNameResolver::createAddressQuery(const std::string& name) {
	return boost::shared_ptr<DomainNameAddressQuery>(new PlatformDomainNameAddressQuery(idnConverter->getIDNAEncoded(name), eventLoop, this));
}

void PlatformDomainNameResolver::run() {
	while (!stopRequested) {
		PlatformDomainNameQuery::ref query;
		{
			boost::unique_lock<boost::mutex> lock(queueMutex);
			while (queue.empty()) {
				queueNonEmpty.wait(lock);
			}
			query = queue.front();
			queue.pop_front();
		}
		// Check whether we don't have a non-null query (used to stop the
		// resolver)
		if (query) {
			query->runBlocking();
		}
	}
}

void PlatformDomainNameResolver::addQueryToQueue(PlatformDomainNameQuery::ref query) {
	{
		boost::lock_guard<boost::mutex> lock(queueMutex);
		queue.push_back(query);
	}
	queueNonEmpty.notify_one();
}

}
