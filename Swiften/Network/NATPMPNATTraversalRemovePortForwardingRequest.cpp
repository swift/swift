/*
 * Copyright (c) 2011 Tobias Markmann
 * Licensed under the simplified BSD license.
 * See Documentation/Licenses/BSD-simplified.txt for more information.
 */

#include "NATPMPNATTraversalRemovePortForwardingRequest.h"

#include <boost/format.hpp>

#include <natpmp.h>

#include <Swiften/Base/foreach.h>
#include <Swiften/Base/Log.h>
#include <Swiften/Network/NetworkInterface.h>
#include <Swiften/Network/PlatformNetworkEnvironment.h>

#pragma GCC diagnostic ignored "-Wold-style-cast"

namespace Swift {

NATPMPNATTraversalRemovePortForwardingRequest::NATPMPNATTraversalRemovePortForwardingRequest(PortMapping mapping, PlatformNATTraversalWorker* worker) : PlatformNATTraversalRequest(worker), mapping(mapping) {

}

NATPMPNATTraversalRemovePortForwardingRequest::~NATPMPNATTraversalRemovePortForwardingRequest() {

}

void NATPMPNATTraversalRemovePortForwardingRequest::runBlocking() {
	boost::optional<bool> result;

	natpmp_t natpmp;
	natpmpresp_t response;
	initnatpmp(&natpmp, 0, 0);

	do {
		if (sendnewportmappingrequest(&natpmp, mapping.protocol == PortMapping::TCP ? NATPMP_PROTOCOL_TCP : NATPMP_PROTOCOL_UDP, 0, 0, mapping.localPort) != 2) {
			SWIFT_LOG(debug) << "Failed to send NAT-PMP remove forwarding request!" << std::endl;
			break;
		}
		int r = 0;

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
			if (response.pnu.newportmapping.privateport == mapping.localPort &&
				response.pnu.newportmapping.mappedpublicport == mapping.publicPort) {
				mapping.leaseInSeconds = response.pnu.newportmapping.lifetime;
				result = boost::optional<bool>(true);
			}
		} else {
			result = boost::optional<bool>(false);
			SWIFT_LOG(debug) << "Inavlid NAT-PMP response." << std::endl;
		}
	} while(false);
	closenatpmp(&natpmp);

	onResult(result);
}

HostAddress NATPMPNATTraversalRemovePortForwardingRequest::getLocalClient() {
	PlatformNetworkEnvironment env;

	foreach (NetworkInterface::ref iface, env.getNetworkInterfaces()) {
		if (!iface->isLoopback()) {
			foreach (HostAddress address, iface->getAddresses()) {
				if (address.getRawAddress().is_v4()) {
					return address;
				}
			}
		}
	}
	return HostAddress();
}

}
