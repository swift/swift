/*
 * Copyright (c) 2011 Tobias Markmann
 * Licensed under the simplified BSD license.
 * See Documentation/Licenses/BSD-simplified.txt for more information.
 */

#include "UPnPNATTraversalForwardPortRequest.h"

#include <boost/format.hpp>

#include <miniupnpc.h>
#include <upnpcommands.h>
#include <upnperrors.h>

#include <Swiften/Base/foreach.h>
#include <Swiften/Network/NetworkInterface.h>
#include <Swiften/Network/PlatformNetworkEnvironment.h>

namespace Swift {

UPnPNATTraversalForwardPortRequest::UPnPNATTraversalForwardPortRequest(NATTraversalForwardPortRequest::PortMapping mapping, PlatformNATTraversalWorker* worker) : PlatformNATTraversalRequest(worker), mapping(mapping) {

}

UPnPNATTraversalForwardPortRequest::~UPnPNATTraversalForwardPortRequest() {

}

void UPnPNATTraversalForwardPortRequest::runBlocking() {
	boost::optional<PortMapping> result;

	UPNPDev* deviceList = 0;
	int error = 0;
	char lanAddrress[64];

	std::string publicPort = str(boost::format("%d") % mapping.publicPort);
	std::string localPort = str(boost::format("%d") % mapping.localPort);
	std::string internalClient = getLocalClient().toString();
	std::string leaseSeconds = str(boost::format("%d") % mapping.leaseInSeconds);
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

		/*
		int ret = UPNP_GetExternalIPAddress(urls.controlURL, data.first.servicetype, externalIPAddress);
		if (ret != UPNPCOMMAND_SUCCESS) {
			break;
		}*/

		int ret = UPNP_AddPortMapping(urls.controlURL, data.first.servicetype, publicPort.c_str(), localPort.c_str(), internalClient.c_str(), 0, mapping.protocol == NATTraversalForwardPortRequest::PortMapping::TCP ? "TCP" : "UDP", 0, leaseSeconds.c_str());
		if (ret == UPNPCOMMAND_SUCCESS) {
			result = boost::optional<NATTraversalForwardPortRequest::PortMapping>(mapping);
		}
	} while(false);

	freeUPNPDevlist(deviceList); deviceList = 0;

	onResult(result);
}

HostAddress UPnPNATTraversalForwardPortRequest::getLocalClient() {
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
