#include "Swiften/Network/DomainNameResolver.h"
#include "Swiften/Base/Platform.h"

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

#include "Swiften/Network/DomainNameResolveException.h"
#include "Swiften/Base/String.h"
#include "Swiften/Base/ByteArray.h"

namespace Swift {

DomainNameResolver::DomainNameResolver() {
}

DomainNameResolver::~DomainNameResolver() {
}

HostAddressPort DomainNameResolver::resolve(const String& domain) {
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

HostAddressPort DomainNameResolver::resolveDomain(const std::string& domain) {
	try {
		return resolveXMPPService(domain);
	}
	catch (const DomainNameResolveException&) {
	}
	return HostAddressPort(resolveHostName(domain), 5222);
}

HostAddressPort DomainNameResolver::resolveXMPPService(const std::string& domain) {
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
			int port = currentEntry->Data.SRV.wPort;
			try {
				// The pNameTarget is actually a PCWSTR, so I would have expected this 
				// conversion to not work at all, but it does.
				// Actually, it doesn't. Fix this and remove explicit cast
				// Remove unicode undef above as well
				std::string hostname((const char*) currentEntry->Data.SRV.pNameTarget);
				HostAddress address = resolveHostName(hostname);
				DnsRecordListFree(responses, DnsFreeRecordList);
				return HostAddressPort(address, port);
			}
			catch (const DomainNameResolveException&) {
			}
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
		int entryLength = dn_skipname(currentEntry, messageEnd);
		currentEntry += entryLength;
		currentEntry += NS_RRFIXEDSZ;

		// Uninteresting information
		currentEntry += 2; // PRIORITY
		currentEntry += 2; // WEIGHT

		// Port
		if (currentEntry >= messageEnd) {
			throw DomainNameResolveException();
		}
		int port = ns_get16(currentEntry);
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
		try {
			// Resolve the hostname
			std::string hostname(entry.getData(), entryLength);
			HostAddress address = resolveHostName(hostname);
			return HostAddressPort(address, port);
		}
		catch (const DomainNameResolveException&) {
		}
		currentEntry += entryLength;
		answersCount--;
	}
#endif

	throw DomainNameResolveException();
}

HostAddress DomainNameResolver::resolveHostName(const std::string& hostname) {
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
