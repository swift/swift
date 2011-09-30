/*
 * Copyright (c) 2011 Tobias Markmann
 * Licensed under the simplified BSD license.
 * See Documentation/Licenses/BSD-simplified.txt for more information.
 */

#include <Swiften/Network/WindowsNetworkEnvironment.h>

#include <string>
#include <vector>
#include <map>
#include <boost/optional.hpp>
#include <Swiften/Network/HostAddress.h>
#include <Swiften/Network/NetworkInterface.h>
#include <Swiften/Base/foreach.h>
#include <Swiften/Base/ByteArray.h>

#include <winsock2.h>
#include <iphlpapi.h>

namespace Swift {

std::vector<NetworkInterface> WindowsNetworkEnvironment::getNetworkInterfaces()  const {
	std::vector<NetworkInterface> result;

	ByteArray adapters;
	ULONG bufferSize = 0;
	ULONG ret;
	ULONG flags = GAA_FLAG_INCLUDE_ALL_INTERFACES | GAA_FLAG_INCLUDE_PREFIX | GAA_FLAG_SKIP_MULTICAST | GAA_FLAG_SKIP_DNS_SERVER;
	while ((ret = GetAdaptersAddresses(AF_UNSPEC, flags, NULL, reinterpret_cast<IP_ADAPTER_ADDRESSES*>(vecptr(adapters)), &bufferSize)) == ERROR_BUFFER_OVERFLOW) {
		adapters.resize(bufferSize);
	};
	if (ret != ERROR_SUCCESS) {
		return result;
	}

	std::map<std::string,NetworkInterface> interfaces;
	for (IP_ADAPTER_ADDRESSES* adapter = reinterpret_cast<IP_ADAPTER_ADDRESSES*>(vecptr(adapters)); adapter; adapter = adapter->Next) {
		std::string name(adapter->AdapterName);
		if (adapter->OperStatus != IfOperStatusUp) {
			continue;
		}
		for (IP_ADAPTER_UNICAST_ADDRESS* address = adapter->FirstUnicastAddress; address; address = address->Next) {
			boost::optional<HostAddress> hostAddress;
			if (address->Address.lpSockaddr->sa_family == PF_INET) {
				sockaddr_in* sa = reinterpret_cast<sockaddr_in*>(address->Address.lpSockaddr);
				hostAddress = HostAddress(reinterpret_cast<const unsigned char*>(&(sa->sin_addr)), 4);
			}
			else if (address->Address.lpSockaddr->sa_family == PF_INET6) {
				sockaddr_in6* sa = reinterpret_cast<sockaddr_in6*>(address->Address.lpSockaddr);
				hostAddress = HostAddress(reinterpret_cast<const unsigned char*>(&(sa->sin6_addr)), 16);
			}
			if (hostAddress) {
				std::map<std::string, NetworkInterface>::iterator i = interfaces.insert(std::make_pair(name, NetworkInterface(name, false))).first;
				i->second.addAddress(*hostAddress);
			}
		}
	}

	for (std::map<std::string,NetworkInterface>::const_iterator i = interfaces.begin(); i != interfaces.end(); ++i) {
		result.push_back(i->second);
	}
	return result;
}

}
