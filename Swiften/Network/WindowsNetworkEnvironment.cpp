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

#include <winsock2.h>
#include <iphlpapi.h>

namespace Swift {

std::vector<NetworkInterface> WindowsNetworkEnvironment::getNetworkInterfaces()  const {
	std::vector<NetworkInterface> result;
	std::map<std::string,NetworkInterface> interfaces;

	IP_ADAPTER_ADDRESSES preBuffer[5];
	PIP_ADAPTER_ADDRESSES adapterStart = preBuffer;

	ULONG bufferSize = sizeof(preBuffer);

	ULONG flags = GAA_FLAG_INCLUDE_ALL_INTERFACES |  GAA_FLAG_INCLUDE_PREFIX | GAA_FLAG_SKIP_MULTICAST | GAA_FLAG_SKIP_DNS_SERVER;

	ULONG ret = GetAdaptersAddresses(	AF_UNSPEC, flags,	NULL, adapterStart, &bufferSize);
	if (ret == ERROR_BUFFER_OVERFLOW) {
		adapterStart = new IP_ADAPTER_ADDRESSES[bufferSize / sizeof(IP_ADAPTER_ADDRESSES)];
		if (!adapterStart) {
			return result;
		}
		ret = GetAdaptersAddresses(AF_UNSPEC, flags, NULL, adapterStart, &bufferSize);
	}
	if (ret != ERROR_SUCCESS) {
		if (adapterStart != preBuffer) {
			delete adapterStart;
		}
		return result;
	}

	for (PIP_ADAPTER_ADDRESSES adapter = adapterStart; adapter; adapter = adapter->Next) {
		std::string name(adapter->AdapterName);

		if (adapter->OperStatus != IfOperStatusUp) {
			continue;
		}

		// iterate over addresses
		for (PIP_ADAPTER_UNICAST_ADDRESS address = adapter->FirstUnicastAddress;  address; address = address->Next) {
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

	if (adapterStart != preBuffer) {
		//delete adapterStart;
	}

	for (std::map<std::string,NetworkInterface>::const_iterator i = interfaces.begin(); i != interfaces.end(); ++i) {
		result.push_back(i->second);
	}
	return result;
}

}
