#include "Swiften/Network/PlatformDomainNameResolver.h"

#include "Swiften/Base/Platform.h"
#include "Swiften/Base/foreach.h"

#include <stdlib.h>
#include <boost/asio.hpp>
#include <idna.h>
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
#include <algorithm>

#include "Swiften/Network/DomainNameResolveException.h"
#include "Swiften/Base/String.h"
#include "Swiften/Base/ByteArray.h"
#include "Swiften/Network/SRVRecord.h"
#include "Swiften/Network/SRVRecordPriorityComparator.h"

namespace Swift {

PlatformDomainNameResolver::PlatformDomainNameResolver() {
}

std::vector<HostAddressPort> PlatformDomainNameResolver::resolve(const String& domain) {
	char* output;
	if (idna_to_ascii_8z(domain.getUTF8Data(), &output, 0) == IDNA_SUCCESS) {
		std::string outputString(output);
		free(output);
		return resolveDomain(outputString);
	}
	else {
		return resolveDomain(domain.getUTF8String());
	}
}

std::vector<HostAddressPort> PlatformDomainNameResolver::resolveDomain(const std::string& domain) {
	try {
		return resolveXMPPService(domain);
	}
	catch (const DomainNameResolveException&) {
	}
	std::vector<HostAddressPort> result;
	result.push_back(HostAddressPort(resolveHostName(domain), 5222));
	return result;
}

std::vector<HostAddressPort> PlatformDomainNameResolver::resolveXMPPService(const std::string& domain) {
	std::vector<SRVRecord> records;

	std::string srvQuery = "_xmpp-client._tcp." + domain;

#if defined(SWIFTEN_PLATFORM_WINDOWS)
	DNS_RECORD* responses;
	// FIXME: This conversion doesn't work if unicode is deffed above
		if (DnsQuery(srvQuery.c_str(), DNS_TYPE_SRV, DNS_QUERY_STANDARD, NULL, &responses, NULL) != ERROR_SUCCESS) {
		throw DomainNameResolveException();
		}

	DNS_RECORD* currentEntry = responses;
	while (currentEntry) {
		if (currentEntry->wType == DNS_TYPE_SRV) {
			SRVRecord record;
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

	ByteArray response;
	response.resize(NS_PACKETSZ);
	int responseLength = res_query(const_cast<char*>(srvQuery.c_str()), ns_c_in, ns_t_srv, reinterpret_cast<u_char*>(response.getData()), response.getSize());
	if (responseLength == -1) {
		throw DomainNameResolveException();
	}

	// Parse header
	HEADER* header = reinterpret_cast<HEADER*>(response.getData());
	unsigned char* messageStart = reinterpret_cast<unsigned char*>(response.getData());
	unsigned char* messageEnd = messageStart + responseLength;
	unsigned char* currentEntry = messageStart + NS_HFIXEDSZ;

	// Skip over the queries
	int queriesCount = ntohs(header->qdcount);
	while (queriesCount > 0) {
		int entryLength = dn_skipname(currentEntry, messageEnd);
		if (entryLength < 0) {
			throw DomainNameResolveException();
		}
		currentEntry += entryLength + NS_QFIXEDSZ;
		queriesCount--;
	}

	// Process the SRV answers
	int answersCount = ntohs(header->ancount);
	while (answersCount > 0) {
		SRVRecord record;

		int entryLength = dn_skipname(currentEntry, messageEnd);
		currentEntry += entryLength;
		currentEntry += NS_RRFIXEDSZ;

		// Priority
		if (currentEntry + 2 >= messageEnd) {
			throw DomainNameResolveException();
		}
		record.priority = ns_get16(currentEntry);
		currentEntry += 2;

		// Weight
		if (currentEntry + 2 >= messageEnd) {
			throw DomainNameResolveException();
		}
		record.weight = ns_get16(currentEntry);
		currentEntry += 2;

		// Port
		if (currentEntry + 2 >= messageEnd) {
			throw DomainNameResolveException();
		}
		record.port = ns_get16(currentEntry);
		currentEntry += 2; 

		// Hostname
		if (currentEntry >= messageEnd) {
			throw DomainNameResolveException();
		}
		ByteArray entry;
		entry.resize(NS_MAXDNAME);
		entryLength = dn_expand(messageStart, messageEnd, currentEntry, entry.getData(), entry.getSize());
		if (entryLength < 0) {
			throw DomainNameResolveException();
		}
		record.hostname = std::string(entry.getData(), entryLength);
		records.push_back(record);
		currentEntry += entryLength;
		answersCount--;
	}
#endif

	// Resolve the hostnames in the records, and build the result list
	std::sort(records.begin(), records.end(), SRVRecordPriorityComparator());
	std::vector<HostAddressPort> result;
	foreach(const SRVRecord& record, records) {
		try {
			HostAddress address = resolveHostName(record.hostname);
			result.push_back(HostAddressPort(address, record.port));
		}
		catch (const DomainNameResolveException&) {
		}
	}
	if (result.empty()) {
		throw DomainNameResolveException();
	}
	return result;
}

HostAddress PlatformDomainNameResolver::resolveHostName(const std::string& hostname) {
	boost::asio::io_service ioService;
	boost::asio::ip::tcp::resolver resolver(ioService);
	boost::asio::ip::tcp::resolver::query query(hostname, "5222");
	try {
		boost::asio::ip::tcp::resolver::iterator endpointIterator = resolver.resolve(query);
		if (endpointIterator == boost::asio::ip::tcp::resolver::iterator()) {
			throw DomainNameResolveException();
		}
		boost::asio::ip::address address = (*endpointIterator).endpoint().address();
		if (address.is_v4()) {
			return HostAddress(&address.to_v4().to_bytes()[0], 4);
		}
		else {
			return HostAddress(&address.to_v6().to_bytes()[0], 16);
		}
	}
	catch (...) {
		throw DomainNameResolveException();
	}
}

}
