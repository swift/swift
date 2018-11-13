/*
 * Copyright (c) 2010-2018 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#pragma GCC diagnostic ignored "-Wold-style-cast"

#include <string>
#include <Swiften/LinkLocal/DNSSD/Bonjour/BonjourQuery.h>
#include <Swiften/LinkLocal/DNSSD/DNSSDResolveHostnameQuery.h>
#include <Swiften/EventLoop/EventLoop.h>
#include <Swiften/Network/HostAddress.h>
#include <boost/numeric/conversion/cast.hpp>

#include <netinet/in.h>

namespace Swift {
    class BonjourQuerier;

    class BonjourResolveHostnameQuery : public DNSSDResolveHostnameQuery, public BonjourQuery {
        public:
            BonjourResolveHostnameQuery(const std::string& hostname, int interfaceIndex, std::shared_ptr<BonjourQuerier> querier, EventLoop* eventLoop) : BonjourQuery(querier, eventLoop) {
                try {
                    DNSServiceErrorType result = DNSServiceGetAddrInfo(
                            &sdRef, 0, boost::numeric_cast<unsigned int>(interfaceIndex), kDNSServiceProtocol_IPv4,
                            hostname.c_str(),
                            &BonjourResolveHostnameQuery::handleHostnameResolvedStatic, this);
                    if (result != kDNSServiceErr_NoError) {
                        sdRef = nullptr;
                    }
                }
                catch (...) {
                    sdRef = nullptr;
                }
            }

            //void DNSSDResolveHostnameQuery::run() {
            void run() {
                if (sdRef) {
                    BonjourQuery::run();
                }
                else {
                    eventLoop->postEvent(boost::bind(boost::ref(onHostnameResolved), boost::optional<HostAddress>()), shared_from_this());
                }
            }

            void finish() {
                BonjourQuery::finish();
            }

        private:
            static void handleHostnameResolvedStatic(DNSServiceRef, DNSServiceFlags, uint32_t, DNSServiceErrorType errorCode, const char*, const struct sockaddr *address, uint32_t, void *context) {
                static_cast<BonjourResolveHostnameQuery*>(context)->handleHostnameResolved(errorCode, address);
            }

            void handleHostnameResolved(DNSServiceErrorType errorCode, const struct sockaddr *rawAddress) {
                if (errorCode) {
                    eventLoop->postEvent(
                                boost::bind(boost::ref(onHostnameResolved),
                                boost::optional<HostAddress>()),
                            shared_from_this());
                }
                else {
                    assert(rawAddress->sa_family == AF_INET);
                    const sockaddr_in* sa = reinterpret_cast<const sockaddr_in*>(rawAddress);
                    uint32_t address = ntohl(sa->sin_addr.s_addr);
                    eventLoop->postEvent(boost::bind(
                                boost::ref(onHostnameResolved),
                                HostAddress(reinterpret_cast<unsigned char*>(&address), 4)),
                            shared_from_this());
                }
            }
    };
}
