/*
 * Copyright (c) 2011 Tobias Markmann
 * Licensed under the simplified BSD license.
 * See Documentation/Licenses/BSD-simplified.txt for more information.
 */

#include "UnixNetworkEnvironment.h"

#include <string>
#include <vector>
#include <map>

#include <boost/shared_ptr.hpp>
#include <boost/smart_ptr/make_shared.hpp>
#include <Swiften/Base/boost_bsignals.h>
#include <Swiften/Network/HostAddress.h>
#include <Swiften/Network/NetworkInterface.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <net/if.h>
#include <ifaddrs.h>

namespace Swift {

std::vector<NetworkInterface::ref> UnixNetworkEnvironment::getNetworkInterfaces() {
	std::map<std::string, UnixNetworkInterface::ref> interfaces;
	std::vector<NetworkInterface::ref> result;

	ifaddrs *addrs = 0;
	int ret = getifaddrs(&addrs);
	if (ret != 0) {
		return result;
	}

	for (ifaddrs *a = addrs; a != 0; a = a->ifa_next) {
		std::string name(a->ifa_name);
		std::string ip;
		if (a->ifa_addr->sa_family == PF_INET) {
			sockaddr_in* sa = reinterpret_cast<sockaddr_in*>(a->ifa_addr);
			char str[INET_ADDRSTRLEN];
			inet_ntop(AF_INET, &(sa->sin_addr), str, INET_ADDRSTRLEN);
			ip.assign(str);
		}
		else if (a->ifa_addr->sa_family == PF_INET6) {
			sockaddr_in6* sa = reinterpret_cast<sockaddr_in6*>(a->ifa_addr);
			char str[INET6_ADDRSTRLEN];
			/*if (IN6_IS_ADDR_LINKLOCAL(sa)) {
				continue;
			}*/
			inet_ntop(AF_INET6, &(sa->sin6_addr), str, INET6_ADDRSTRLEN);
			ip.assign(str);
		}
		if (!ip.empty()) {
			if (interfaces.find(name) == interfaces.end()) {
				interfaces[name] = boost::make_shared<UnixNetworkInterface>(name);
				if (a->ifa_flags & IFF_LOOPBACK) {
					interfaces[name]->setLoopback(true);
				}
			}
			interfaces[name]->addHostAddress(HostAddress(ip));
		}
	}

	freeifaddrs(addrs);

	for(std::map<std::string, UnixNetworkInterface::ref>::iterator i = interfaces.begin(); i != interfaces.end(); ++i) {
		result.push_back(i->second);
	}
	return result;
}

}
