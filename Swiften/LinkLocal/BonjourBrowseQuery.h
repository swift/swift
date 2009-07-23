#pragma once

#include "Swiften/LinkLocal/BonjourQuery.h"
#include "Swiften/LinkLocal/DNSSDBrowseQuery.h"

namespace Swift {
	class BonjourQuerier;

	class BonjourBrowseQuery : public DNSSDBrowseQuery, public BonjourQuery {
		public:	
			BonjourBrowseQuery(boost::shared_ptr<BonjourQuerier> q) : BonjourQuery(q) {
				DNSServiceErrorType result = DNSServiceBrowse(
						&sdRef, 0, 0, "_presence._tcp", 0, 
						&BonjourBrowseQuery::handleServiceDiscoveredGlobal , this);
				if (result != kDNSServiceErr_NoError) {
					// TODO
				}
			}

			void startBrowsing() {
				assert(sdRef);
				run();
			}

			void stopBrowsing() {
				finish();
			}

		private:
			static void handleServiceDiscoveredGlobal(DNSServiceRef, DNSServiceFlags flags, uint32_t interfaceIndex, DNSServiceErrorType errorCode, const char *serviceName, const char *regtype, const char *replyDomain, void *context) {
				static_cast<BonjourBrowseQuery*>(context)->handleServiceDiscovered(flags, interfaceIndex, errorCode, serviceName, regtype, replyDomain);
			}

			void handleServiceDiscovered(
					DNSServiceFlags flags, 
					uint32_t interfaceIndex, 
					DNSServiceErrorType errorCode, 
					const char *serviceName, 
					const char *type, 
					const char *domain) {
				if (errorCode != kDNSServiceErr_NoError) {
					return;
				}
				else {
					LinkLocalServiceID service(serviceName, type, domain, interfaceIndex);
					if (flags & kDNSServiceFlagsAdd) {
						onServiceAdded(service);
					}
					else {
						onServiceRemoved(service);
					}
				}
		}
	};
}
