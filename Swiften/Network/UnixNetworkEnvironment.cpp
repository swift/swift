/*
 * Copyright (c) 2011 Tobias Markmann
 * Licensed under the simplified BSD license.
 * See Documentation/Licenses/BSD-simplified.txt for more information.
 */

#include <Swiften/Network/UnixNetworkEnvironment.h>

#include <string>
#include <vector>
#include <map>
#include <boost/optional.hpp>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <net/if.h>
#include <ifaddrs.h>

#include <Swiften/Base/boost_bsignals.h>
#include <Swiften/Network/HostAddress.h>
#include <Swiften/Network/NetworkInterface.h>

namespace Swift {

std::vector<NetworkInterface> UnixNetworkEnvironment::getNetworkInterfaces() const {
	std::map<std::string, NetworkInterface> interfaces;

	ifaddrs* addrs = 0;
	int ret = getifaddrs(&addrs);
	if (ret != 0) {
		return std::vector<NetworkInterface>();
	}

	for (ifaddrs* a = addrs; a != 0; a = a->ifa_next) {
		std::string name(a->ifa_name);
		boost::optional<HostAddress> address;
		if (a->ifa_addr->sa_family == PF_INET) {
			sockaddr_in* sa = reinterpret_cast<sockaddr_in*>(a->ifa_addr);
			address = HostAddress(reinterpret_cast<const unsigned char*>(&(sa->sin_addr)), 4);
		}
		else if (a->ifa_addr->sa_family == PF_INET6) {
			sockaddr_in6* sa = reinterpret_cast<sockaddr_in6*>(a->ifa_addr);
			address = HostAddress(reinterpret_cast<const unsigned char*>(&(sa->sin6_addr)), 16);
		}
		if (address) {
			std::map<std::string, NetworkInterface>::iterator i = interfaces.insert(std::make_pair(name, NetworkInterface(name, a->ifa_flags & IFF_LOOPBACK))).first;
			i->second.addAddress(*address);
		}
	}

	freeifaddrs(addrs);

	std::vector<NetworkInterface> result;
	for (std::map<std::string,NetworkInterface>::const_iterator i = interfaces.begin(); i != interfaces.end(); ++i) {
		result.push_back(i->second);
	}
	return result;
}

}
