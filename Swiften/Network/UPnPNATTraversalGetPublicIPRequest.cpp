/*
 * Copyright (c) 2011 Tobias Markmann
 * Licensed under the simplified BSD license.
 * See Documentation/Licenses/BSD-simplified.txt for more information.
 */

#include "UPnPNATTraversalGetPublicIPRequest.h"

#include <miniupnpc.h>
#include <upnpcommands.h>
#include <upnperrors.h>

namespace Swift {

UPnPNATTraversalGetPublicIPRequest::UPnPNATTraversalGetPublicIPRequest(PlatformNATTraversalWorker* worker) : PlatformNATTraversalGetPublicIPRequest(worker) {

}

UPnPNATTraversalGetPublicIPRequest::~UPnPNATTraversalGetPublicIPRequest() {

}

void UPnPNATTraversalGetPublicIPRequest::runBlocking() {
	boost::optional<HostAddress> result;

	UPNPDev* deviceList = 0;
	int error = 0;
	char lanAddrress[64];
	char externalIPAddress[40];
	UPNPUrls urls;
	IGDdatas data;

	do {
		// find valid IGD
		deviceList = upnpDiscover(1500 /* timeout in ms */, 0, 0, 0, 0 /* do IPv6? */, &error);
		if (!deviceList) {
			break;
		}

		if (!UPNP_GetValidIGD(deviceList, &urls, &data, lanAddrress, sizeof(lanAddrress))) {
			break;
		}

		int ret = UPNP_GetExternalIPAddress(urls.controlURL, data.first.servicetype, externalIPAddress);
		if (ret != UPNPCOMMAND_SUCCESS) {
			break;
		} else {
			result = HostAddress(std::string(externalIPAddress));
		}
	} while(false);

	freeUPNPDevlist(deviceList); deviceList = 0;

	onResult(result);
}

}
