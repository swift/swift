#pragma once

#include <boost/enable_shared_from_this.hpp>
#include <boost/thread.hpp>
#include <boost/thread/mutex.hpp>
#include <dns_sd.h>

#include "Swiften/LinkLocal/DNSSDService.h"
#include "Swiften/EventLoop/EventOwner.h"

namespace Swift {
	class AppleDNSSDService : public DNSSDService, public EventOwner, public boost::enable_shared_from_this<AppleDNSSDService> {
		public:
			AppleDNSSDService();
			~AppleDNSSDService();

			virtual void start();
			virtual void stop();

			virtual void registerService(const String& name, int port, const LinkLocalServiceInfo&);
			virtual void updateService(const LinkLocalServiceInfo&);
			virtual void unregisterService();

			virtual void startResolvingService(const Service&);
			virtual void stopResolvingService(const Service&);

			virtual void resolveHostname(const String& hostname, int interfaceIndex = 0);

		private:
			void doStart();
			void interruptSelect();

			static void handleServiceDiscoveredGlobal(DNSServiceRef, DNSServiceFlags, uint32_t, DNSServiceErrorType, const char *, const char *, const char *, void*);
			void handleServiceDiscovered(DNSServiceRef, DNSServiceFlags, uint32_t, DNSServiceErrorType, const char *, const char *, const char *);
			static void handleServiceRegisteredGlobal(DNSServiceRef, DNSServiceFlags, DNSServiceErrorType, const char *, const char *, const char *, void *);
			void handleServiceRegistered(DNSServiceRef, DNSServiceFlags, DNSServiceErrorType, const char *, const char *, const char *);
			static void handleServiceResolvedGlobal(DNSServiceRef, DNSServiceFlags, uint32_t, DNSServiceErrorType, const char *, const char *, uint16_t, uint16_t, const unsigned char *, void *);
			void handleServiceResolved(DNSServiceRef, DNSServiceFlags, uint32_t, DNSServiceErrorType, const char *, const char *, uint16_t, uint16_t, const unsigned char *);
			static void handleHostnameResolvedGlobal(DNSServiceRef sdRef, DNSServiceFlags flags, uint32_t interfaceIndex, DNSServiceErrorType errorCode, const char *hostname, const struct sockaddr *address, uint32_t ttl, void *context);
			void handleHostnameResolved(DNSServiceRef sdRef, DNSServiceFlags flags, uint32_t interfaceIndex, DNSServiceErrorType errorCode, const char *hostname, const struct sockaddr *address, uint32_t ttl);

		private:
			boost::thread* thread;
			bool stopRequested;
			bool haveError;
			int interruptSelectReadSocket;
			int interruptSelectWriteSocket;
			boost::mutex sdRefsMutex;
			DNSServiceRef browseSDRef;
			DNSServiceRef registerSDRef;
			typedef std::map<Service, DNSServiceRef> ServiceSDRefMap;
			ServiceSDRefMap resolveSDRefs;
			typedef std::vector<DNSServiceRef> HostnameSDRefs;
			HostnameSDRefs hostnameResolveSDRefs;
	};
}
