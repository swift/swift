#pragma once

#include "Swiften/LinkLocal/BonjourQuery.h"
#include "Swiften/LinkLocal/DNSSDRegisterQuery.h"
#include "Swiften/LinkLocal/LinkLocalServiceInfo.h"
#include "Swiften/Base/ByteArray.h"
#include "Swiften/EventLoop/MainEventLoop.h"

namespace Swift {
	class BonjourQuerier;

	class BonjourRegisterQuery : public DNSSDRegisterQuery, public BonjourQuery {
		public:	
			BonjourRegisterQuery(const String& name, int port, const LinkLocalServiceInfo& info, boost::shared_ptr<BonjourQuerier> querier) : BonjourQuery(querier) {
				ByteArray txtRecord = info.toTXTRecord();
				DNSServiceErrorType result = DNSServiceRegister(
						&sdRef, 0, 0, name.getUTF8Data(), "_presence._tcp", NULL, NULL, port, 
						txtRecord.getSize(), txtRecord.getData(), 
						&BonjourRegisterQuery::handleServiceRegisteredStatic, this);
				if (result != kDNSServiceErr_NoError) {
					// TODO
					std::cerr << "Error creating service registration" << std::endl;
				}
			}

			void registerService() {
				run();
			}

			void updateServiceInfo(const LinkLocalServiceInfo& info) {
				boost::lock_guard<boost::mutex> lock(sdRefMutex);
				ByteArray txtRecord = info.toTXTRecord();
				DNSServiceUpdateRecord(sdRef, NULL, NULL, txtRecord.getSize(), txtRecord.getData(), 0);
			}

		private:
			static void handleServiceRegisteredStatic(DNSServiceRef, DNSServiceFlags, DNSServiceErrorType errorCode, const char *name, const char *regtype, const char *domain, void *context) {
				static_cast<BonjourRegisterQuery*>(context)->handleServiceRegistered(errorCode, name, regtype, domain);
			}

			void handleServiceRegistered(DNSServiceErrorType errorCode, const char *name, const char *regtype, const char *domain) {
				if (errorCode != kDNSServiceErr_NoError) {
					MainEventLoop::postEvent(boost::bind(boost::ref(onRegisterFinished), boost::optional<LinkLocalServiceID>()), shared_from_this());
				}
				else {
					MainEventLoop::postEvent(boost::bind(boost::ref(onRegisterFinished), boost::optional<LinkLocalServiceID>(LinkLocalServiceID(name, regtype, domain, 0))), shared_from_this());
				}
			}
	};
}
