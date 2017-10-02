/*
 * Copyright (c) 2011 Tobias Markmann
 * Licensed under the simplified BSD license.
 * See Documentation/Licenses/BSD-simplified.txt for more information.
 */

/*
 * Copyright (c) 2014-2016 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#include <Swiften/Network/NATPMPInterface.h>

#include <memory>

#include <boost/numeric/conversion/cast.hpp>

#include <Swiften/Base/Log.h>

// This has to be included after the previous headers, because of WIN32 macro
// being defined somewhere.
#include <natpmp.h>

#pragma GCC diagnostic ignored "-Wold-style-cast"

namespace Swift {

struct NATPMPInterface::Private {
    natpmp_t natpmp;
};

NATPMPInterface::NATPMPInterface() : p(new Private()) {
    initnatpmp(&p->natpmp, 0, 0);
}

NATPMPInterface::~NATPMPInterface() {
    closenatpmp(&p->natpmp);
}

bool NATPMPInterface::isAvailable() {
    return getPublicIP() ? true : false;
}

boost::optional<HostAddress> NATPMPInterface::getPublicIP() {
    if (sendpublicaddressrequest(&p->natpmp) < 0) {
        SWIFT_LOG(debug) << "Failed to send NAT-PMP public address request!" << std::endl;
        return boost::optional<HostAddress>();
    }

    int r = 0;
    natpmpresp_t response;
    do {
      fd_set fds;
      struct timeval timeout;
      FD_ZERO(&fds);
      FD_SET(p->natpmp.s, &fds);
      getnatpmprequesttimeout(&p->natpmp, &timeout);

      // Limit NAT-PMP timeout to ten seconds.
      timeout.tv_sec = 10;
      timeout.tv_usec = 0;

      select(FD_SETSIZE, &fds, nullptr, nullptr, &timeout);
      r = readnatpmpresponseorretry(&p->natpmp, &response);
    } while (false /*r == NATPMP_TRYAGAIN*/);

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
    if (sendnewportmappingrequest(
                &p->natpmp,
                mapping.getProtocol() == NATPortMapping::TCP ? NATPMP_PROTOCOL_TCP : NATPMP_PROTOCOL_UDP,
                boost::numeric_cast<uint16_t>(mapping.getLocalPort()),
                boost::numeric_cast<uint16_t>(mapping.getPublicPort()),
                boost::numeric_cast<uint32_t>(mapping.getLeaseInSeconds())) < 0) {
        SWIFT_LOG(debug) << "Failed to send NAT-PMP port forwarding request!" << std::endl;
        return boost::optional<NATPortMapping>();
    }

    int r = 0;
    natpmpresp_t response;
    do {
        fd_set fds;
        struct timeval timeout;
        FD_ZERO(&fds);
        FD_SET(p->natpmp.s, &fds);
        getnatpmprequesttimeout(&p->natpmp, &timeout);

        // Limit NAT-PMP timeout to ten seconds.
        timeout.tv_sec = 10;
        timeout.tv_usec = 0;

        select(FD_SETSIZE, &fds, nullptr, nullptr, &timeout);
        r = readnatpmpresponseorretry(&p->natpmp, &response);
    } while(false /*r == NATPMP_TRYAGAIN*/);

    if (r == 0) {
        NATPortMapping result(response.pnu.newportmapping.privateport, response.pnu.newportmapping.mappedpublicport, NATPortMapping::TCP, boost::numeric_cast<int>(response.pnu.newportmapping.lifetime));
        return result;
    }
    else {
        SWIFT_LOG(debug) << "Invalid NAT-PMP response." << std::endl;
        return boost::optional<NATPortMapping>();
    }
}

bool NATPMPInterface::removePortForward(const NATPortMapping& mapping) {
    if (sendnewportmappingrequest(&p->natpmp, mapping.getProtocol() == NATPortMapping::TCP ? NATPMP_PROTOCOL_TCP : NATPMP_PROTOCOL_UDP, 0, 0, boost::numeric_cast<uint32_t>(mapping.getLocalPort())) < 0) {
        SWIFT_LOG(debug) << "Failed to send NAT-PMP remove forwarding request!" << std::endl;
        return false;
    }

    int r = 0;
    natpmpresp_t response;
    do {
      fd_set fds;
      struct timeval timeout;
      FD_ZERO(&fds);
      FD_SET(p->natpmp.s, &fds);
      getnatpmprequesttimeout(&p->natpmp, &timeout);
      select(FD_SETSIZE, &fds, nullptr, nullptr, &timeout);
      r = readnatpmpresponseorretry(&p->natpmp, &response);
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
