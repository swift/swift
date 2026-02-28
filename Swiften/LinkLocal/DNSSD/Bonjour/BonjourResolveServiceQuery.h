/*
 * Copyright (c) 2010-2018 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <boost/numeric/conversion/cast.hpp>

#include <Swiften/Base/ByteArray.h>
#include <Swiften/EventLoop/EventLoop.h>
#include <Swiften/LinkLocal/DNSSD/Bonjour/BonjourQuery.h>
#include <Swiften/LinkLocal/DNSSD/DNSSDResolveServiceQuery.h>
#include <Swiften/LinkLocal/LinkLocalServiceInfo.h>

namespace Swift {
    class BonjourQuerier;

    class BonjourResolveServiceQuery : public DNSSDResolveServiceQuery, public BonjourQuery {
        public:
            BonjourResolveServiceQuery(const DNSSDServiceID& service, std::shared_ptr<BonjourQuerier> querier, EventLoop* eventLoop) : BonjourQuery(querier, eventLoop) {
                try {
                    DNSServiceErrorType result = DNSServiceResolve(
                            &sdRef, 0, boost::numeric_cast<unsigned int>(service.getNetworkInterfaceID()),
                            service.getName().c_str(), service.getType().c_str(),
                            service.getDomain().c_str(),
                            &BonjourResolveServiceQuery::handleServiceResolvedStatic, this);
                    if (result != kDNSServiceErr_NoError) {
                        sdRef = nullptr;
                    }
                }
                catch (...) {
                    sdRef = nullptr;
                }
            }

            void start() {
                if (sdRef) {
                    run();
                }
                else {
                    eventLoop->postEvent(boost::bind(boost::ref(onServiceResolved), boost::optional<Result>()), shared_from_this());
                }
            }

            void stop() {
                finish();
            }

        private:
            static void handleServiceResolvedStatic(DNSServiceRef, DNSServiceFlags, uint32_t, DNSServiceErrorType errorCode, const char *fullname, const char *hosttarget, uint16_t port, uint16_t txtLen, const unsigned char *txtRecord, void *context) {
                static_cast<BonjourResolveServiceQuery*>(context)->handleServiceResolved(errorCode, fullname, hosttarget, port, txtLen, txtRecord);
            }

            void handleServiceResolved(DNSServiceErrorType errorCode, const char* fullName, const char* host, uint16_t port, uint16_t txtLen, const unsigned char *txtRecord) {
                if (errorCode != kDNSServiceErr_NoError) {
                    eventLoop->postEvent(boost::bind(boost::ref(onServiceResolved), boost::optional<Result>()), shared_from_this());
                }
                else {
                    //std::cout << "Service resolved: name:" << fullName << " host:" << host << " port:" << port << std::endl;
                    eventLoop->postEvent(
                            boost::bind(
                                boost::ref(onServiceResolved),
                                Result(std::string(fullName), std::string(host), port,
                                    createByteArray(reinterpret_cast<const char*>(txtRecord), txtLen))),
                            shared_from_this());
                }
            }
    };
}
