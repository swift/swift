/*
 * Copyright (c) 2010 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#include <boost/asio.hpp>

#include <Swiften/Network/PlatformDomainNameServiceQuery.h>

#pragma GCC diagnostic ignored "-Wold-style-cast"

#include <Swiften/Base/Platform.h>
#include <stdlib.h>
#ifdef SWIFTEN_PLATFORM_WINDOWS
#undef UNICODE
#include <windows.h>
#include <windns.h>
#ifndef DNS_TYPE_SRV
#define DNS_TYPE_SRV 33
#endif
#else
#include <arpa/nameser.h>
#include <arpa/nameser_compat.h>
#include <resolv.h>
#endif
#include <boost/bind.hpp>

#include <Swiften/Base/ByteArray.h>
#include <Swiften/EventLoop/EventLoop.h>
#include <Swiften/Base/foreach.h>
#include <Swiften/Base/Log.h>
#include <Swiften/Network/PlatformDomainNameResolver.h>

using namespace Swift;

namespace Swift {

PlatformDomainNameServiceQuery::PlatformDomainNameServiceQuery(const std::string& service, EventLoop* eventLoop, PlatformDomainNameResolver* resolver) : PlatformDomainNameQuery(resolver), eventLoop(eventLoop), service(service) {
}

void PlatformDomainNameServiceQuery::run() {
	getResolver()->addQueryToQueue(shared_from_this());
}

void PlatformDomainNameServiceQuery::runBlocking() {
	SWIFT_LOG(debug) << "Querying " << service << std::endl;

	std::vector<DomainNameServiceQuery::Result> records;

#if defined(SWIFTEN_PLATFORM_WINDOWS)
	DNS_RECORD* responses;
	// FIXME: This conversion doesn't work if unicode is deffed above
	if (DnsQuery(service.c_str(), DNS_TYPE_SRV, DNS_QUERY_STANDARD, NULL, &responses, NULL) != ERROR_SUCCESS) {
		emitError();
		return;
	}

	DNS_RECORD* currentEntry = responses;
	while (currentEntry) {
		if (currentEntry->wType == DNS_TYPE_SRV) {
			DomainNameServiceQuery::Result record;
			record.priority = currentEntry->Data.SRV.wPriority;
			record.weight = currentEntry->Data.SRV.wWeight;
			record.port = currentEntry->Data.SRV.wPort;
				
			// The pNameTarget is actually a PCWSTR, so I would have expected this 
			// conversion to not work at all, but it does.
			// Actually, it doesn't. Fix this and remove explicit cast
			// Remove unicode undef above as well
			record.hostname = std::string((const char*) currentEntry->Data.SRV.pNameTarget);
			records.push_back(record);
		}
		currentEntry = currentEntry->pNext;
	}
	DnsRecordListFree(responses, DnsFreeRecordList);

#else
	// Make sure we reinitialize the domain list every time
	res_init();

	ByteArray response;
	response.resize(NS_PACKETSZ);
	int responseLength = res_query(const_cast<char*>(service.c_str()), ns_c_in, ns_t_srv, reinterpret_cast<u_char*>(vecptr(response)), response.size());
	if (responseLength == -1) {
		SWIFT_LOG(debug) << "Error" << std::endl;
		emitError();
		return;
	}

	// Parse header
	HEADER* header = reinterpret_cast<HEADER*>(vecptr(response));
	unsigned char* messageStart = vecptr(response);
	unsigned char* messageEnd = messageStart + responseLength;
	unsigned char* currentEntry = messageStart + NS_HFIXEDSZ;

	// Skip over the queries
	int queriesCount = ntohs(header->qdcount);
	while (queriesCount > 0) {
		int entryLength = dn_skipname(currentEntry, messageEnd);
		if (entryLength < 0) {
			emitError();
			return;
		}
		currentEntry += entryLength + NS_QFIXEDSZ;
		queriesCount--;
	}

	// Process the SRV answers
	int answersCount = ntohs(header->ancount);
	while (answersCount > 0) {
		DomainNameServiceQuery::Result record;

		int entryLength = dn_skipname(currentEntry, messageEnd);
		currentEntry += entryLength;
		currentEntry += NS_RRFIXEDSZ;

		// Priority
		if (currentEntry + 2 >= messageEnd) {
			emitError();
			return;
		}
		record.priority = ns_get16(currentEntry);
		currentEntry += 2;

		// Weight
		if (currentEntry + 2 >= messageEnd) {
			emitError();
			return;
		}
		record.weight = ns_get16(currentEntry);
		currentEntry += 2;

		// Port
		if (currentEntry + 2 >= messageEnd) {
			emitError();
			return;
		}
		record.port = ns_get16(currentEntry);
		currentEntry += 2; 

		// Hostname
		if (currentEntry >= messageEnd) {
			emitError();
			return;
		}
		ByteArray entry;
		entry.resize(NS_MAXDNAME);
		entryLength = dn_expand(messageStart, messageEnd, currentEntry, reinterpret_cast<char*>(vecptr(entry)), entry.size());
		if (entryLength < 0) {
			emitError();
			return;
		}
		record.hostname = std::string(reinterpret_cast<const char*>(vecptr(entry)));
		records.push_back(record);
		currentEntry += entryLength;
		answersCount--;
	}
#endif

	std::sort(records.begin(), records.end(), ResultPriorityComparator());
	//std::cout << "Sending out " << records.size() << " SRV results " << std::endl;
	eventLoop->postEvent(boost::bind(boost::ref(onResult), records), shared_from_this());
}

void PlatformDomainNameServiceQuery::emitError() {
	eventLoop->postEvent(boost::bind(boost::ref(onResult), std::vector<DomainNameServiceQuery::Result>()), shared_from_this());
}

}
