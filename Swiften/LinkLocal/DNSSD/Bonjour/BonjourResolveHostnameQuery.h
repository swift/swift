#pragma once

#include "Swiften/Base/String.h"
#include "Swiften/LinkLocal/DNSSD/Bonjour/BonjourQuery.h"
#include "Swiften/LinkLocal/DNSSD/DNSSDResolveHostnameQuery.h"
#include "Swiften/EventLoop/MainEventLoop.h"
#include "Swiften/Network/HostAddress.h"

#include <netinet/in.h>

namespace Swift {
	class BonjourQuerier;

	class BonjourResolveHostnameQuery : public DNSSDResolveHostnameQuery, public BonjourQuery {
		public: 
			BonjourResolveHostnameQuery(const String& hostname, int interfaceIndex, boost::shared_ptr<BonjourQuerier> querier) : BonjourQuery(querier) {
				DNSServiceErrorType result = DNSServiceGetAddrInfo(
						&sdRef, 0, interfaceIndex, kDNSServiceProtocol_IPv4, 
						hostname.getUTF8Data(), 
						&BonjourResolveHostnameQuery::handleHostnameResolvedStatic, this);
				if (result != kDNSServiceErr_NoError) {
					sdRef = NULL;
				}
			}

			//void DNSSDResolveHostnameQuery::run() {
			void run() {
				if (sdRef) {
					BonjourQuery::run();
				}
				else {
					MainEventLoop::postEvent(boost::bind(boost::ref(onHostnameResolved), boost::optional<HostAddress>()), shared_from_this());
				}
			}

			void finish() {
				BonjourQuery::finish();
			}

		private:
			static void handleHostnameResolvedStatic(DNSServiceRef, DNSServiceFlags, uint32_t, DNSServiceErrorType errorCode, const char*, const struct sockaddr *address, uint32_t, void *context) {
				static_cast<BonjourResolveHostnameQuery*>(context)->handleHostnameResolved(errorCode, address);
			} 

			void handleHostnameResolved(DNSServiceErrorType errorCode, const struct sockaddr *rawAddress) {
				if (errorCode) {
					MainEventLoop::postEvent(
								boost::bind(boost::ref(onHostnameResolved), 
								boost::optional<HostAddress>()), 
							shared_from_this());
				}
				else {
					assert(rawAddress->sa_family == AF_INET);
					const sockaddr_in* sa = reinterpret_cast<const sockaddr_in*>(rawAddress);
					uint32_t address = ntohl(sa->sin_addr.s_addr);
					MainEventLoop::postEvent(boost::bind(
								boost::ref(onHostnameResolved), 
								HostAddress(reinterpret_cast<unsigned char*>(&address), 4)), 
							shared_from_this());
				}
			}
	};
}
