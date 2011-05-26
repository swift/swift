/*
 * Copyright (c) 2011 Tobias Markmann
 * Licensed under the simplified BSD license.
 * See Documentation/Licenses/BSD-simplified.txt for more information.
 */

#include "NATPMPNATTraversalGetPublicIPRequest.h"

#include <natpmp.h>

#include <Swiften/Base/Log.h>

namespace Swift {

NATPMPNATTraversalGetPublicIPRequest::NATPMPNATTraversalGetPublicIPRequest(PlatformNATTraversalWorker* worker) : PlatformNATTraversalGetPublicIPRequest(worker) {

}

NATPMPNATTraversalGetPublicIPRequest::~NATPMPNATTraversalGetPublicIPRequest() {

}

/*
TODO: a non-blocking solution should be possible too here
void NATPMPNATTraversalGetPublicIPRequest::run() {
	// we can run directly since libnatpmp's API is asynchronous
	runBlocking();
}*/

void NATPMPNATTraversalGetPublicIPRequest::runBlocking() {
	boost::optional<HostAddress> result;

	natpmp_t natpmp;
	natpmpresp_t response;
	initnatpmp(&natpmp, 0, 0);

	do {
		if (sendpublicaddressrequest(&natpmp) != 2) {
			SWIFT_LOG(debug) << "Failed to send NAT-PMP public address request!" << std::endl;
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
			result = boost::optional<HostAddress>(HostAddress(reinterpret_cast<const unsigned char*>(&(response.pnu.publicaddress.addr)), 4));
		} else {
			SWIFT_LOG(debug) << "Inavlid NAT-PMP response." << std::endl;
		}
	} while(false);
	closenatpmp(&natpmp);
	onResult(result);
}

}
