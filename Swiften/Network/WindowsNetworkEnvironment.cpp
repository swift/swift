/*
 * Copyright (c) 2011 Tobias Markmann
 * Licensed under the simplified BSD license.
 * See Documentation/Licenses/BSD-simplified.txt for more information.
 */

#include "WindowsNetworkEnvironment.h"

#include <string>
#include <vector>
#include <map>

#include <boost/shared_ptr.hpp>
#include <boost/smart_ptr/make_shared.hpp>
#include <Swiften/Base/boost_bsignals.h>
#include <Swiften/Network/HostAddress.h>
#include <Swiften/Network/NetworkInterface.h>

#include <winsock2.h>
#include <iphlpapi.h>

namespace Swift {

std::string winSocketAddressToStdString(const SOCKET_ADDRESS& socketAddress) {
	char text[46];
	ULONG bufferSize = sizeof(text);
	std::string result;

	int ret = WSAAddressToString(socketAddress.lpSockaddr, socketAddress.iSockaddrLength, NULL, text, &bufferSize);
	if (ret == 0) {
		result.assign(text, sizeof(text));
	}
	return result;
}

std::vector<NetworkInterface::ref> WindowsNetworkEnvironment::getNetworkInterfaces() {
	std::map<std::string, WindowsNetworkInterface::ref> interfaces;
	std::vector<NetworkInterface::ref> result;

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
			std::string ip;

			if (address->Address.lpSockaddr->sa_family == PF_INET ||
					address->Address.lpSockaddr->sa_family == PF_INET6) {
				ip = winSocketAddressToStdString(address->Address);
				if (!ip.empty()) {
					if (interfaces.find(name) == interfaces.end()) {
						interfaces[name] = boost::make_shared<WindowsNetworkInterface>();
						interfaces[name]->setName(name);
					}
					interfaces[name]->addHostAddress(HostAddress(ip));
				}
			}
		}
	}

	if (adapterStart != preBuffer) {
		//delete adapterStart;
	}

	for(std::map<std::string, WindowsNetworkInterface::ref>::iterator i = interfaces.begin(); i != interfaces.end(); ++i) {
		result.push_back(i->second);
	}
	return result;
}

}
