/*
 * Copyright (c) 2011 Tobias Markmann
 * Licensed under the simplified BSD license.
 * See Documentation/Licenses/BSD-simplified.txt for more information.
 */

#include <Swiften/Network/NATPMPInterface.h>

#include <Swiften/Base/Log.h>

#pragma GCC diagnostic ignored "-Wold-style-cast"

namespace Swift {

NATPMPInterface::NATPMPInterface() {
	initnatpmp(&natpmp, 0, 0);
}

NATPMPInterface::~NATPMPInterface() {
	closenatpmp(&natpmp);
}

bool NATPMPInterface::isAvailable() {
	return getPublicIP();
}

boost::optional<HostAddress> NATPMPInterface::getPublicIP() {
	if (sendpublicaddressrequest(&natpmp) < 0) {
		SWIFT_LOG(debug) << "Failed to send NAT-PMP public address request!" << std::endl;
		return boost::optional<HostAddress>();
	}

	int r = 0;
	natpmpresp_t response;
	do {
	  fd_set fds;
	  struct timeval timeout;
	  FD_ZERO(&fds);
	  FD_SET(natpmp.s, &fds);
	  getnatpmprequesttimeout(&natpmp, &timeout);
	  select(FD_SETSIZE, &fds, NULL, NULL, &timeout);
	  r = readnatpmpresponseorretry(&natpmp, &response);
	} while (r == NATPMP_TRYAGAIN);

	if (r == 0) {
		return boost::optional<HostAddress>(HostAddress(reinterpret_cast<const unsigned char*>(&(response.pnu.publicaddress.addr)), 4));
	}
	else {
		SWIFT_LOG(debug) << "Inavlid NAT-PMP response." << std::endl;
		return boost::optional<HostAddress>();
	}
}

boost::optional<NATPortMapping> NATPMPInterface::addPortForward(int localPort, int publicPort) {
	NATPortMapping mapping(localPort, publicPort, NATPortMapping::TCP);
	if (sendnewportmappingrequest(&natpmp, mapping.getProtocol() == NATPortMapping::TCP ? NATPMP_PROTOCOL_TCP : NATPMP_PROTOCOL_UDP, mapping.getLeaseInSeconds(), mapping.getPublicPort(), mapping.getLocalPort()) < 0) {
			SWIFT_LOG(debug) << "Failed to send NAT-PMP port forwarding request!" << std::endl;
			return boost::optional<NATPortMapping>();
	}

	int r = 0;
	natpmpresp_t response;
	do {
		fd_set fds;
		struct timeval timeout;
		FD_ZERO(&fds);
		FD_SET(natpmp.s, &fds);
		getnatpmprequesttimeout(&natpmp, &timeout);
		select(FD_SETSIZE, &fds, NULL, NULL, &timeout);
		r = readnatpmpresponseorretry(&natpmp, &response);
	} while(r == NATPMP_TRYAGAIN);

	if (r == 0) {
		NATPortMapping result(response.pnu.newportmapping.privateport, response.pnu.newportmapping.mappedpublicport, NATPortMapping::TCP, response.pnu.newportmapping.lifetime);
		return result;
	}
	else {
		SWIFT_LOG(debug) << "Invalid NAT-PMP response." << std::endl;
		return boost::optional<NATPortMapping>();
	}
}

bool NATPMPInterface::removePortForward(const NATPortMapping& mapping) {
	if (sendnewportmappingrequest(&natpmp, mapping.getProtocol() == NATPortMapping::TCP ? NATPMP_PROTOCOL_TCP : NATPMP_PROTOCOL_UDP, 0, 0, mapping.getLocalPort()) < 0) {
		SWIFT_LOG(debug) << "Failed to send NAT-PMP remove forwarding request!" << std::endl;
		return false;
	}

	int r = 0;
	natpmpresp_t response;
	do {
	  fd_set fds;
	  struct timeval timeout;
	  FD_ZERO(&fds);
	  FD_SET(natpmp.s, &fds);
	  getnatpmprequesttimeout(&natpmp, &timeout);
	  select(FD_SETSIZE, &fds, NULL, NULL, &timeout);
	  r = readnatpmpresponseorretry(&natpmp, &response);
	} while(r == NATPMP_TRYAGAIN);

	if (r == 0) {
		return true;
	}
	else {
		SWIFT_LOG(debug) << "Invalid NAT-PMP response." << std::endl;
		return false;
	}
}


}
