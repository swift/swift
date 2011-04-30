/*
 * Copyright (c) 2010 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
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
#include <Swiften/IDN/IDNA.h>
#include <Swiften/Network/HostAddress.h>
#include <Swiften/EventLoop/EventLoop.h>
#include <Swiften/Network/HostAddressPort.h>
#include <Swiften/Network/DomainNameAddressQuery.h>
#include <Swiften/Network/PlatformDomainNameAddressQuery.h>

using namespace Swift;

namespace Swift {

PlatformDomainNameResolver::PlatformDomainNameResolver(EventLoop* eventLoop) : eventLoop(eventLoop), stopRequested(false) {
	thread = new boost::thread(boost::bind(&PlatformDomainNameResolver::run, this));
}

PlatformDomainNameResolver::~PlatformDomainNameResolver() {
	stopRequested = true;
	addQueryToQueue(boost::shared_ptr<PlatformDomainNameQuery>());
	thread->join();
	delete thread;
}

boost::shared_ptr<DomainNameServiceQuery> PlatformDomainNameResolver::createServiceQuery(const std::string& name) {
	return boost::shared_ptr<DomainNameServiceQuery>(new PlatformDomainNameServiceQuery(IDNA::getEncoded(name), eventLoop, this));
}

boost::shared_ptr<DomainNameAddressQuery> PlatformDomainNameResolver::createAddressQuery(const std::string& name) {
	return boost::shared_ptr<DomainNameAddressQuery>(new PlatformDomainNameAddressQuery(IDNA::getEncoded(name), eventLoop, this));
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
