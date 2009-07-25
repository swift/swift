#pragma once

#include "Swiften/LinkLocal/DNSSD/Bonjour/BonjourQuery.h"
#include "Swiften/LinkLocal/DNSSD/DNSSDBrowseQuery.h"
#include "Swiften/EventLoop/MainEventLoop.h"

namespace Swift {
	class BonjourQuerier;

	class BonjourBrowseQuery : public DNSSDBrowseQuery, public BonjourQuery {
		public:	
			BonjourBrowseQuery(boost::shared_ptr<BonjourQuerier> q) : BonjourQuery(q) {
				DNSServiceErrorType result = DNSServiceBrowse(
						&sdRef, 0, 0, "_presence._tcp", 0, 
						&BonjourBrowseQuery::handleServiceDiscoveredStatic, this);
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
			static void handleServiceDiscoveredStatic(DNSServiceRef, DNSServiceFlags flags, uint32_t interfaceIndex, DNSServiceErrorType errorCode, const char *name, const char *type, const char *domain, void *context) {
				static_cast<BonjourBrowseQuery*>(context)->handleServiceDiscovered(flags, interfaceIndex, errorCode, name, type, domain);
			}

			void handleServiceDiscovered(DNSServiceFlags flags, uint32_t interfaceIndex, DNSServiceErrorType errorCode, const char *name, const char *type, const char *domain) {
				if (errorCode != kDNSServiceErr_NoError) {
					return;
				}
				else {
					DNSSDServiceID service(name, type, domain, interfaceIndex);
					if (flags & kDNSServiceFlagsAdd) {
						MainEventLoop::postEvent(boost::bind(boost::ref(onServiceAdded), service), shared_from_this());
					}
					else {
						MainEventLoop::postEvent(boost::bind(boost::ref(onServiceRemoved), service), shared_from_this());
					}
				}
			}
	};
}
