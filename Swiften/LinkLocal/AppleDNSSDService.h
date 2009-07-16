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

			virtual void registerService(const String& name, int port, const std::map<String,String>& properties);
			virtual void unregisterService();
			virtual void start();
			virtual void stop();

		private:
			void doStart();
			void interruptSelect();

			static void handleServiceDiscoveredGlobal(DNSServiceRef, DNSServiceFlags, uint32_t, DNSServiceErrorType, const char *, const char *, const char *, void*);
			void handleServiceDiscovered(DNSServiceRef, DNSServiceFlags, uint32_t, DNSServiceErrorType, const char *, const char *, const char *);
			static void handleServiceRegisteredGlobal(DNSServiceRef, DNSServiceFlags, DNSServiceErrorType, const char *, const char *, const char *, void *);
			void handleServiceRegistered(DNSServiceRef, DNSServiceFlags, DNSServiceErrorType, const char *, const char *, const char *);

		private:
			boost::thread* thread;
			bool stopRequested;
			int interruptSelectReadSocket;
			int interruptSelectWriteSocket;
			boost::mutex sdRefsMutex;
			DNSServiceRef browseSDRef;
			DNSServiceRef registerSDRef;
	};
}
