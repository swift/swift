/*
 * Copyright (c) 2010-2018 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <boost/numeric/conversion/cast.hpp>

#include <Swiften/EventLoop/EventLoop.h>
#include <Swiften/LinkLocal/DNSSD/Bonjour/BonjourQuery.h>
#include <Swiften/LinkLocal/DNSSD/DNSSDBrowseQuery.h>

namespace Swift {
    class BonjourQuerier;

    class BonjourBrowseQuery : public DNSSDBrowseQuery, public BonjourQuery {
        public:
            BonjourBrowseQuery(std::shared_ptr<BonjourQuerier> q, EventLoop* eventLoop) : BonjourQuery(q, eventLoop) {
                DNSServiceErrorType result = DNSServiceBrowse(
                        &sdRef, 0, 0, "_presence._tcp", nullptr,
                        &BonjourBrowseQuery::handleServiceDiscoveredStatic, this);
                if (result != kDNSServiceErr_NoError) {
                    sdRef = nullptr;
                }
            }

            void startBrowsing() {
                if (!sdRef) {
                    eventLoop->postEvent(boost::bind(boost::ref(onError)), shared_from_this());
                }
                else {
                    run();
                }
            }

            void stopBrowsing() {
                finish();
            }

        private:
            static void handleServiceDiscoveredStatic(DNSServiceRef, DNSServiceFlags flags, uint32_t interfaceIndex, DNSServiceErrorType errorCode, const char *name, const char *type, const char *domain, void *context) {
                static_cast<BonjourBrowseQuery*>(context)->handleServiceDiscovered(flags, interfaceIndex, errorCode, name, type, domain);
            }

            void handleServiceDiscovered(DNSServiceFlags flags, uint32_t interfaceIndex, DNSServiceErrorType errorCode, const char *name, const char *type, const char *domain) {
                if (errorCode != kDNSServiceErr_NoError) {
                    eventLoop->postEvent(boost::bind(boost::ref(onError)), shared_from_this());
                }
                else {
                    //std::cout << "Discovered service: name:" << name << " domain:" << domain << " type: " << type << std::endl;
                    try {
                        DNSSDServiceID service(name, domain, type, boost::numeric_cast<int>(interfaceIndex));
                        if (flags & kDNSServiceFlagsAdd) {
                            eventLoop->postEvent(boost::bind(boost::ref(onServiceAdded), service), shared_from_this());
                        }
                        else {
                            eventLoop->postEvent(boost::bind(boost::ref(onServiceRemoved), service), shared_from_this());
                        }
                    }
                    catch (...) {
                        eventLoop->postEvent(boost::bind(boost::ref(onError)), shared_from_this());
                    }
                }
            }
    };
}
