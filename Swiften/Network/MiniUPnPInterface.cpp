/*
 * Copyright (c) 2011 Tobias Markmann
 * Licensed under the simplified BSD license.
 * See Documentation/Licenses/BSD-simplified.txt for more information.
 */

#include <Swiften/Network/MiniUPnPInterface.h>

#include <miniupnpc.h>
#include <upnpcommands.h>
#include <upnperrors.h>
#include <boost/smart_ptr/make_shared.hpp>
#include <boost/lexical_cast.hpp>

#include <Swiften/Base/Log.h>

namespace Swift {

struct MiniUPnPInterface::Private {
	bool isValid;
	std::string localAddress;
	UPNPDev* deviceList;
	UPNPUrls urls;
	IGDdatas data;
};

MiniUPnPInterface::MiniUPnPInterface() : p(boost::make_shared<Private>()) {
	p->isValid = false;
	int error = 0;
	p->deviceList = upnpDiscover(1500 /* timeout in ms */, 0, 0, 0, 0 /* do IPv6? */, &error);
	if (!p->deviceList) {
		return;
	}

	char lanAddress[64];
	if (!UPNP_GetValidIGD(p->deviceList, &p->urls, &p->data, lanAddress, sizeof(lanAddress))) {
		return;
	}
	p->localAddress = std::string(lanAddress);
	p->isValid = true;
}

MiniUPnPInterface::~MiniUPnPInterface() {
	if (p->isValid) {
		FreeUPNPUrls(&p->urls);
	}
	freeUPNPDevlist(p->deviceList);
}

boost::optional<HostAddress> MiniUPnPInterface::getPublicIP() {
	if (!p->isValid) {
		return boost::optional<HostAddress>();
	}
	char externalIPAddress[40];
	int ret = UPNP_GetExternalIPAddress(p->urls.controlURL, p->data.first.servicetype, externalIPAddress);
	if (ret != UPNPCOMMAND_SUCCESS) {
		return boost::optional<HostAddress>();
	}
	else {
		return HostAddress(std::string(externalIPAddress));
	}
}

boost::optional<NATPortMapping> MiniUPnPInterface::addPortForward(int actualLocalPort, int actualPublicPort) {
	if (!p->isValid) {
		return boost::optional<NATPortMapping>();
	}

	NATPortMapping mapping(actualLocalPort, actualPublicPort, NATPortMapping::TCP);

	std::string publicPort = boost::lexical_cast<std::string>(mapping.getPublicPort());
	std::string localPort = boost::lexical_cast<std::string>(mapping.getLocalPort());
	std::string leaseSeconds = boost::lexical_cast<std::string>(mapping.getLeaseInSeconds());

	int ret = UPNP_AddPortMapping(p->urls.controlURL, p->data.first.servicetype, publicPort.c_str(), localPort.c_str(), p->localAddress.c_str(), 0, mapping.getPublicPort() == NATPortMapping::TCP ? "TCP" : "UDP", 0, leaseSeconds.c_str());
	if (ret == UPNPCOMMAND_SUCCESS) {
		return mapping;
	}
	else {
		return boost::optional<NATPortMapping>();
	}
}

bool MiniUPnPInterface::removePortForward(const NATPortMapping& mapping) {
	if (!p->isValid) {
		return false;
	}

	std::string publicPort = boost::lexical_cast<std::string>(mapping.getPublicPort());
	std::string localPort = boost::lexical_cast<std::string>(mapping.getLocalPort());
	std::string leaseSeconds = boost::lexical_cast<std::string>(mapping.getLeaseInSeconds());

	int ret = UPNP_DeletePortMapping(p->urls.controlURL, p->data.first.servicetype, publicPort.c_str(), mapping.getProtocol() == NATPortMapping::TCP ? "TCP" : "UDP", 0);
	return ret == UPNPCOMMAND_SUCCESS;
}

bool MiniUPnPInterface::isAvailable() {
	return p->isValid;
}

}
