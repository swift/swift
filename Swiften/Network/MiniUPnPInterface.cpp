/*
 * Copyright (c) 2011 Tobias Markmann
 * Licensed under the simplified BSD license.
 * See Documentation/Licenses/BSD-simplified.txt for more information.
 */

/*
 * Copyright (c) 2015-2016 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#include <Swiften/Network/MiniUPnPInterface.h>

#include <memory>

#include <boost/lexical_cast.hpp>

#include <miniupnpc.h>
#include <upnpcommands.h>
#include <upnperrors.h>

#include <Swiften/Base/Log.h>

namespace Swift {

struct MiniUPnPInterface::Private {
    bool isValid;
    std::string localAddress;
    UPNPDev* deviceList;
    UPNPUrls urls;
    IGDdatas data;
};

MiniUPnPInterface::MiniUPnPInterface() : p(new Private()) {
    p->isValid = false;
    int error = 0;
#if MINIUPNPC_API_VERSION > 13
    p->deviceList = upnpDiscover(1500 /* timeout in ms */, nullptr, nullptr, 0, 0 /* do IPv6? */, 2 /* default TTL */, &error);
#else
    p->deviceList = upnpDiscover(1500 /* timeout in ms */, nullptr, nullptr, 0, 0 /* do IPv6? */, &error);
#endif
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
        return HostAddress::fromString(std::string(externalIPAddress));
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

    int ret = UPNP_AddPortMapping(
            p->urls.controlURL,
            p->data.first.servicetype,
            publicPort.c_str(),
            localPort.c_str(),
            p->localAddress.c_str(),
            "Swift",
            mapping.getProtocol() == NATPortMapping::TCP ? "TCP" : "UDP",
            nullptr,
            leaseSeconds.c_str());
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

    int ret = UPNP_DeletePortMapping(p->urls.controlURL, p->data.first.servicetype, publicPort.c_str(), mapping.getProtocol() == NATPortMapping::TCP ? "TCP" : "UDP", nullptr);
    return ret == UPNPCOMMAND_SUCCESS;
}

bool MiniUPnPInterface::isAvailable() {
    return p->isValid;
}

}
