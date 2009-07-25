#pragma once

#include "Swiften/LinkLocal/BonjourQuery.h"
#include "Swiften/LinkLocal/DNSSDResolveServiceQuery.h"
#include "Swiften/LinkLocal/LinkLocalServiceInfo.h"
#include "Swiften/Base/ByteArray.h"
#include "Swiften/EventLoop/MainEventLoop.h"

namespace Swift {
	class BonjourQuerier;

	class BonjourResolveServiceQuery : public DNSSDResolveServiceQuery, public BonjourQuery {
		public:	
			BonjourResolveServiceQuery(const LinkLocalServiceID& service, boost::shared_ptr<BonjourQuerier> querier) : BonjourQuery(querier) {
				DNSServiceErrorType result = DNSServiceResolve(
						&sdRef, 0, service.getNetworkInterfaceID(), 
						service.getName().getUTF8Data(), service.getType().getUTF8Data(), 
						service.getDomain().getUTF8Data(), 
						&BonjourResolveServiceQuery::handleServiceResolvedStatic, this);
				if (result != kDNSServiceErr_NoError) {
					MainEventLoop::postEvent(boost::bind(boost::ref(onServiceResolved), boost::optional<Result>()), shared_from_this());
				}
			}

			void start() {
				run();
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
					MainEventLoop::postEvent(boost::bind(boost::ref(onServiceResolved), boost::optional<Result>()), shared_from_this());
				}
				else {
					MainEventLoop::postEvent(
							boost::bind(
								boost::ref(onServiceResolved), 
								Result(String(fullName), String(host), port, 
									LinkLocalServiceInfo::createFromTXTRecord(
										ByteArray(reinterpret_cast<const char*>(txtRecord), txtLen)))), 
							shared_from_this());
				}
			}
	};
}
