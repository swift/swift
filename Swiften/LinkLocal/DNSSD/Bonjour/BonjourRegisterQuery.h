/*
 * Copyright (c) 2010-2018 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <mutex>

#include <boost/numeric/conversion/cast.hpp>

#include <Swiften/Base/ByteArray.h>
#include <Swiften/EventLoop/EventLoop.h>
#include <Swiften/LinkLocal/DNSSD/Bonjour/BonjourQuery.h>
#include <Swiften/LinkLocal/DNSSD/DNSSDRegisterQuery.h>

namespace Swift {
    class BonjourQuerier;

    class BonjourRegisterQuery : public DNSSDRegisterQuery, public BonjourQuery {
        public:
            BonjourRegisterQuery(const std::string& name, unsigned short port, const ByteArray& txtRecord, std::shared_ptr<BonjourQuerier> querier, EventLoop* eventLoop) : BonjourQuery(querier, eventLoop) {
                unsigned short recordSize = 0;
                try {
                    recordSize = boost::numeric_cast<unsigned short>(txtRecord.size());
                }
                catch (const boost::numeric::bad_numeric_cast&) {
                    SWIFT_LOG(warning) << "Bonjour TXT record is too long (" << txtRecord.size() << " bytes), not registring service" << std::endl;
                    return;
                }
                DNSServiceErrorType result = DNSServiceRegister(
                        &sdRef, 0, 0, name.c_str(), "_presence._tcp", nullptr, nullptr, port,
                        recordSize, vecptr(txtRecord),
                        &BonjourRegisterQuery::handleServiceRegisteredStatic, this);
                if (result != kDNSServiceErr_NoError) {
                    SWIFT_LOG(warning) << "Failed to register Bonjour service" << std::endl;
                    sdRef = nullptr;
                }
            }

            void registerService() {
                if (sdRef) {
                    run();
                }
                else {
                    eventLoop->postEvent(boost::bind(boost::ref(onRegisterFinished), boost::optional<DNSSDServiceID>()), shared_from_this());
                }
            }

            void unregisterService() {
                finish();
            }

            void updateServiceInfo(const ByteArray& txtRecord) {
                std::lock_guard<std::mutex> lock(sdRefMutex);
                try {
                    DNSServiceUpdateRecord(sdRef, nullptr, 0, boost::numeric_cast<unsigned short>(txtRecord.size()), vecptr(txtRecord), 0);
                }
                catch (const boost::numeric::bad_numeric_cast&) {
                    SWIFT_LOG(warning) << "Bonjour TXT record is too long (" << txtRecord.size() << " bytes), not updating service record" << std::endl;
                }
            }

        private:
            static void handleServiceRegisteredStatic(DNSServiceRef, DNSServiceFlags, DNSServiceErrorType errorCode, const char *name, const char *regtype, const char *domain, void *context) {
                static_cast<BonjourRegisterQuery*>(context)->handleServiceRegistered(errorCode, name, regtype, domain);
            }

            void handleServiceRegistered(DNSServiceErrorType errorCode, const char *name, const char *regtype, const char *domain) {
                if (errorCode != kDNSServiceErr_NoError) {
                    eventLoop->postEvent(boost::bind(boost::ref(onRegisterFinished), boost::optional<DNSSDServiceID>()), shared_from_this());
                }
                else {
                    eventLoop->postEvent(boost::bind(boost::ref(onRegisterFinished), boost::optional<DNSSDServiceID>(DNSSDServiceID(name, domain, regtype, 0))), shared_from_this());
                }
            }
    };
}
