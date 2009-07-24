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
						&BonjourBrowseQuery::handleServiceDiscovered, this);
				if (result != kDNSServiceErr_NoError) {
					std::cout << "Error" << std::endl;
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
			static void handleServiceDiscovered(DNSServiceRef, DNSServiceFlags flags, uint32_t interfaceIndex, DNSServiceErrorType errorCode, const char *name, const char *type, const char *domain, void *context) {
				if (errorCode != kDNSServiceErr_NoError) {
					return;
				}
				else {
					BonjourBrowseQuery* query = static_cast<BonjourBrowseQuery*>(context);
					LinkLocalServiceID service(name, type, domain, interfaceIndex);
					std::cout << "Service discovered: " << name << std::endl;
					if (flags & kDNSServiceFlagsAdd) {
						query->onServiceAdded(service);
					}
					else {
						query->onServiceRemoved(service);
					}
				}
			}
	};
}
