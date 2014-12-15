/*
 * Copyright (c) 2010-2014 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <boost/numeric/conversion/cast.hpp>
#include <boost/thread.hpp>

#include <Swiften/Base/ByteArray.h>
#include <Swiften/EventLoop/EventLoop.h>
#include <Swiften/LinkLocal/DNSSD/Bonjour/BonjourQuery.h>
#include <Swiften/LinkLocal/DNSSD/DNSSDRegisterQuery.h>

namespace Swift {
	class BonjourQuerier;

	class BonjourRegisterQuery : public DNSSDRegisterQuery, public BonjourQuery {
		public:
			BonjourRegisterQuery(const std::string& name, int port, const ByteArray& txtRecord, boost::shared_ptr<BonjourQuerier> querier, EventLoop* eventLoop) : BonjourQuery(querier, eventLoop) {
				DNSServiceErrorType result = DNSServiceRegister(
						&sdRef, 0, 0, name.c_str(), "_presence._tcp", NULL, NULL, boost::numeric_cast<unsigned short>(port),
						boost::numeric_cast<unsigned short>(txtRecord.size()), vecptr(txtRecord),
						&BonjourRegisterQuery::handleServiceRegisteredStatic, this);
				if (result != kDNSServiceErr_NoError) {
					sdRef = NULL;
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
				boost::lock_guard<boost::mutex> lock(sdRefMutex);
				DNSServiceUpdateRecord(sdRef, NULL, 0, boost::numeric_cast<unsigned short>(txtRecord.size()), vecptr(txtRecord), 0);
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
