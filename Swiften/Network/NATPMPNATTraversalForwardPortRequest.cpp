/*
 * Copyright (c) 2011 Tobias Markmann
 * Licensed under the simplified BSD license.
 * See Documentation/Licenses/BSD-simplified.txt for more information.
 */

#include "NATPMPNATTraversalForwardPortRequest.h"

#include <natpmp.h>

#include <Swiften/Base/Log.h>

namespace Swift {

NATPMPNATTraversalForwardPortRequest::NATPMPNATTraversalForwardPortRequest(PlatformNATTraversalForwardPortRequest::PortMapping mapping, PlatformNATTraversalWorker* worker) : PlatformNATTraversalForwardPortRequest(worker), mapping(mapping) {

}

NATPMPNATTraversalForwardPortRequest::~NATPMPNATTraversalForwardPortRequest() {

}

void NATPMPNATTraversalForwardPortRequest::runBlocking() {
	boost::optional<PortMapping> result;

	natpmp_t natpmp;
	natpmpresp_t response;
	initnatpmp(&natpmp, 0, 0);

	do {
		if (sendnewportmappingrequest(&natpmp, mapping.protocol == PortMapping::TCP ? NATPMP_PROTOCOL_TCP : NATPMP_PROTOCOL_UDP, mapping.leaseInSeconds, mapping.publicPort, mapping.localPort) != 2) {
			SWIFT_LOG(debug) << "Failed to send NAT-PMP port forwarding request!" << std::endl;
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
				result = boost::optional<PortMapping>(mapping);
			}
		} else {
			SWIFT_LOG(debug) << "Inavlid NAT-PMP response." << std::endl;
		}
	} while(false);
	closenatpmp(&natpmp);

	onResult(result);
}

}
