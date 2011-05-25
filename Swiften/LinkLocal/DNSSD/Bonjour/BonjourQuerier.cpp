/*
 * Copyright (c) 2010 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#include <Swiften/LinkLocal/DNSSD/Bonjour/BonjourQuerier.h>

#include <unistd.h>
#include <sys/socket.h>
#include <fcntl.h>

#include <Swiften/LinkLocal/DNSSD/Bonjour/BonjourBrowseQuery.h>
#include <Swiften/LinkLocal/DNSSD/Bonjour/BonjourRegisterQuery.h>
#include <Swiften/LinkLocal/DNSSD/Bonjour/BonjourResolveServiceQuery.h>
#include <Swiften/LinkLocal/DNSSD/Bonjour/BonjourResolveHostnameQuery.h>
#include <Swiften/Base/foreach.h>
#include <Swiften/Base/Algorithm.h>

namespace Swift {

BonjourQuerier::BonjourQuerier(EventLoop* eventLoop) : eventLoop(eventLoop), stopRequested(false), thread(0) {
	int fds[2];
	int result = pipe(fds);
	assert(result == 0);
	(void) result;
	interruptSelectReadSocket = fds[0];
	fcntl(interruptSelectReadSocket, F_SETFL, fcntl(interruptSelectReadSocket, F_GETFL)|O_NONBLOCK);
	interruptSelectWriteSocket = fds[1];
}

BonjourQuerier::~BonjourQuerier() {
	assert(!thread);
}

boost::shared_ptr<DNSSDBrowseQuery> BonjourQuerier::createBrowseQuery() {
	return boost::shared_ptr<DNSSDBrowseQuery>(new BonjourBrowseQuery(shared_from_this(), eventLoop));
}

boost::shared_ptr<DNSSDRegisterQuery> BonjourQuerier::createRegisterQuery(const std::string& name, int port, const ByteArray& info) {
	return boost::shared_ptr<DNSSDRegisterQuery>(new BonjourRegisterQuery(name, port, info, shared_from_this(), eventLoop));
}

boost::shared_ptr<DNSSDResolveServiceQuery> BonjourQuerier::createResolveServiceQuery(const DNSSDServiceID& service) {
	return boost::shared_ptr<DNSSDResolveServiceQuery>(new BonjourResolveServiceQuery(service, shared_from_this(), eventLoop));
}

boost::shared_ptr<DNSSDResolveHostnameQuery> BonjourQuerier::createResolveHostnameQuery(const std::string& hostname, int interfaceIndex) {
	return boost::shared_ptr<DNSSDResolveHostnameQuery>(new BonjourResolveHostnameQuery(hostname, interfaceIndex, shared_from_this(), eventLoop));
}

void BonjourQuerier::addRunningQuery(boost::shared_ptr<BonjourQuery> query) {
	{
		boost::lock_guard<boost::mutex> lock(runningQueriesMutex);
		runningQueries.push_back(query);
	}
	runningQueriesAvailableEvent.notify_one();
	interruptSelect();
}

void BonjourQuerier::removeRunningQuery(boost::shared_ptr<BonjourQuery> query) {
	{
		boost::lock_guard<boost::mutex> lock(runningQueriesMutex);
		erase(runningQueries, query);
	}
}

void BonjourQuerier::interruptSelect() {
	char c = 0;
	write(interruptSelectWriteSocket, &c, 1);
}

void BonjourQuerier::start() {
	assert(!thread);
	thread = new boost::thread(boost::bind(&BonjourQuerier::run, shared_from_this()));
}

void BonjourQuerier::stop() {
	if (thread) {
		stopRequested = true;
		assert(runningQueries.empty());
		runningQueriesAvailableEvent.notify_one();
		interruptSelect();
		thread->join();
		delete thread;
		thread = NULL;
		stopRequested = false;
	}
}

void BonjourQuerier::run() {
	while (!stopRequested) {
		fd_set fdSet;
		int maxSocket;
		{
			boost::unique_lock<boost::mutex> lock(runningQueriesMutex);
			if (runningQueries.empty()) {
				runningQueriesAvailableEvent.wait(lock);
				if (runningQueries.empty()) {
					continue;
				}
			}

			// Run all running queries
			FD_ZERO(&fdSet);
			maxSocket = interruptSelectReadSocket;
			FD_SET(interruptSelectReadSocket, &fdSet);

			foreach(const boost::shared_ptr<BonjourQuery>& query, runningQueries) {
				int socketID = query->getSocketID();
				maxSocket = std::max(maxSocket, socketID);
				FD_SET(socketID, &fdSet);
			}
		}

		if (select(maxSocket+1, &fdSet, NULL, NULL, 0) <= 0) {
			continue;
		}

		if (FD_ISSET(interruptSelectReadSocket, &fdSet)) {
			char dummy;
			while (read(interruptSelectReadSocket, &dummy, 1) > 0) {}
		}

		{
			boost::lock_guard<boost::mutex> lock(runningQueriesMutex);
			foreach(boost::shared_ptr<BonjourQuery> query, runningQueries) {
				if (FD_ISSET(query->getSocketID(), &fdSet)) {
					query->processResult();
				}
			}
		}
	}
}

}
