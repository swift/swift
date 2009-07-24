#pragma once

#include "Swiften/LinkLocal/BonjourQuery.h"
#include "Swiften/LinkLocal/DNSSDPublishQuery.h"
#include "Swiften/LinkLocal/LinkLocalServiceInfo.h"
#include "Swiften/Base/ByteArray.h"
#include "Swiften/EventLoop/MainEventLoop.h"

namespace Swift {
	class BonjourQuerier;

	class BonjourPublishQuery : public DNSSDPublishQuery, public BonjourQuery {
		public:	
			BonjourPublishQuery(const String& name, int port, const LinkLocalServiceInfo& info, boost::shared_ptr<BonjourQuerier> querier) : BonjourQuery(querier) {
				ByteArray txtRecord = info.toTXTRecord();
				DNSServiceErrorType result = DNSServiceRegister(
						&sdRef, 0, 0, name.getUTF8Data(), "_presence._tcp", NULL, NULL, port, 
						txtRecord.getSize(), txtRecord.getData(), 
						&BonjourPublishQuery::handleServicePublishedStatic, this);
				if (result != kDNSServiceErr_NoError) {
					// TODO
					std::cerr << "Error creating service registration" << std::endl;
				}
			}

			void publish() {
				run();
			}

			void update(const LinkLocalService& info) {
				boost::lock_guard<boost::mutex> lock(sdRefMutex);
				ByteArray txtRecord = info.toTXTRecord();
				DNSServiceUpdateRecord(sdRef, NULL, NULL, txtRecord.getSize(), txtRecord.getData(), 0);
			}

		private:
			static void handleServicePublishedStatic(DNSServiceRef, DNSServiceFlags, DNSServiceErrorType errorCode, const char *name, const char *regtype, const char *domain, void *context) {
				static_cast<BonjourPublishQuery*>(context)->handleServicePublished(errorCode, name, regtype, domain);
			}

			void handleServicePublished(DNSServiceErrorType errorCode, const char *name, const char *regtype, const char *domain) {
				if (errorCode != kDNSServiceErr_NoError) {
					MainEventLoop::postEvent(boost::bind(boost::ref(onPublishFinished), boost::optional<LinkLocalServiceID>()), shared_from_this());
				}
				else {
					MainEventLoop::postEvent(boost::bind(boost::ref(onPublishFinished), boost::optional<LinkLocalServiceID>(LinkLocalServiceID(name, regtype, domain, 0))), shared_from_this());
				}
			}
	};
}
