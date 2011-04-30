/*
 * Copyright (c) 2010 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

// TODO: Check the second param of postEvent. We sometimes omit it. Same 
// goes for the PlatformDomainNameResolver.

#include <Swiften/Network/CAresDomainNameResolver.h>
#include <Swiften/Base/Platform.h>

#ifndef SWIFTEN_PLATFORM_WINDOWS
#include <netdb.h>
#include <arpa/inet.h>
#endif
#include <algorithm>

#include <Swiften/Network/DomainNameServiceQuery.h>
#include <Swiften/Network/DomainNameAddressQuery.h>
#include <Swiften/Base/ByteArray.h>
#include <Swiften/EventLoop/EventLoop.h>
#include <Swiften/Base/foreach.h>

namespace Swift {

class CAresQuery : public boost::enable_shared_from_this<CAresQuery>, public EventOwner {
	public:
		CAresQuery(const std::string& query, int dnsclass, int type, CAresDomainNameResolver* resolver) : query(query), dnsclass(dnsclass), type(type), resolver(resolver) {
		}

		virtual ~CAresQuery() {
		}

		void addToQueue() {
			resolver->addToQueue(shared_from_this());
		}

		void doRun(ares_channel* channel) {
			ares_query(*channel, query.c_str(), dnsclass, type, &CAresQuery::handleResult, this);
		}

		static void handleResult(void* arg, int status, int timeouts, unsigned char* buffer, int len) {
			reinterpret_cast<CAresQuery*>(arg)->handleResult(status, timeouts, buffer, len);
		}

		virtual void handleResult(int status, int, unsigned char* buffer, int len) = 0;
	
	private:
		std::string query;
		int dnsclass;
		int type;
		CAresDomainNameResolver* resolver;
};

class CAresDomainNameServiceQuery : public DomainNameServiceQuery, public CAresQuery {
	public:
		CAresDomainNameServiceQuery(const std::string& service, CAresDomainNameResolver* resolver) : CAresQuery(service, 1, 33, resolver) {
		}

		virtual void run() {
			addToQueue();
		}

		void handleResult(int status, int, unsigned char* buffer, int len) {
			if (status == ARES_SUCCESS) {
				std::vector<DomainNameServiceQuery::Result> records;
				ares_srv_reply* rawRecords;
				if (ares_parse_srv_reply(buffer, len, &rawRecords) == ARES_SUCCESS) {
					for( ; rawRecords != NULL; rawRecords = rawRecords->next) {
						DomainNameServiceQuery::Result record;
						record.priority = rawRecords->priority;
						record.weight = rawRecords->weight;
						record.port = rawRecords->port;
						record.hostname = std::string(rawRecords->host);
						records.push_back(record);
					}
				}
				std::sort(records.begin(), records.end(), ResultPriorityComparator());
				eventLoop->postEvent(boost::bind(boost::ref(onResult), records)); 
			}
			else if (status != ARES_EDESTRUCTION) {
				eventLoop->postEvent(boost::bind(boost::ref(onResult), std::vector<DomainNameServiceQuery::Result>()), shared_from_this());
			}
		}
};

class CAresDomainNameAddressQuery : public DomainNameAddressQuery, public CAresQuery {
	public:
		CAresDomainNameAddressQuery(const std::string& host, CAresDomainNameResolver* resolver) : CAresQuery(host, 1, 1, resolver)  {
		}
	
		virtual void run() {
			addToQueue();
		}

		void handleResult(int status, int, unsigned char* buffer, int len) {
			if (status == ARES_SUCCESS) {
				struct hostent* hosts;
				if (ares_parse_a_reply(buffer, len, &hosts, NULL, NULL) == ARES_SUCCESS) {
					// Check whether the different fields are what we expect them to be
					struct in_addr addr;
					addr.s_addr = *(unsigned int*)hosts->h_addr_list[0];

					std::vector<HostAddress> results;
					results.push_back(HostAddress(inet_ntoa(addr)));

					eventLoop->postEvent(boost::bind(boost::ref(onResult), results, boost::optional<DomainNameResolveError>()), boost::dynamic_pointer_cast<CAresDomainNameAddressQuery>(shared_from_this())); 
					ares_free_hostent(hosts);
				}
				else {
					eventLoop->postEvent(boost::bind(boost::ref(onResult), std::vector<HostAddress>(), boost::optional<DomainNameResolveError>(DomainNameResolveError())), shared_from_this());
				}
			}
			else if (status != ARES_EDESTRUCTION) {
				eventLoop->postEvent(boost::bind(boost::ref(onResult), std::vector<HostAddress>(), boost::optional<DomainNameResolveError>(DomainNameResolveError())), shared_from_this());
			}
		}
};

CAresDomainNameResolver::CAresDomainNameResolver() : stopRequested(false) {
	ares_init(&channel);
	thread = new boost::thread(boost::bind(&CAresDomainNameResolver::run, this));
}

CAresDomainNameResolver::~CAresDomainNameResolver() {
	stopRequested = true;
	thread->join();
	ares_destroy(channel);
}

boost::shared_ptr<DomainNameServiceQuery> CAresDomainNameResolver::createServiceQuery(const std::string& name) {
	return boost::shared_ptr<DomainNameServiceQuery>(new CAresDomainNameServiceQuery(getNormalized(name), this));
}

boost::shared_ptr<DomainNameAddressQuery> CAresDomainNameResolver::createAddressQuery(const std::string& name) {
	return boost::shared_ptr<DomainNameAddressQuery>(new CAresDomainNameAddressQuery(getNormalized(name), this));
}

void CAresDomainNameResolver::addToQueue(boost::shared_ptr<CAresQuery> query) {
	boost::lock_guard<boost::mutex> lock(pendingQueriesMutex);
	pendingQueries.push_back(query);
}

void CAresDomainNameResolver::run() {
	fd_set readers, writers;
	struct timeval timeout;
	timeout.tv_sec = 0;
	timeout.tv_usec = 100000;
	while(!stopRequested) {
		{
			boost::unique_lock<boost::mutex> lock(pendingQueriesMutex);
			foreach(const boost::shared_ptr<CAresQuery>& query, pendingQueries) {
				query->doRun(&channel);
			}
			pendingQueries.clear();
		}
		FD_ZERO(&readers);
		FD_ZERO(&writers);
		int nfds = ares_fds(channel, &readers, &writers);
		//if (nfds) {
		//	break;
		//}
		struct timeval tv;
		struct timeval* tvp = ares_timeout(channel, &timeout, &tv);
		select(nfds, &readers, &writers, NULL, tvp);
		ares_process(channel, &readers, &writers);
	}
}

}
