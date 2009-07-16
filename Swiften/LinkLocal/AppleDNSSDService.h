#pragma once

#include <boost/enable_shared_from_this.hpp>
#include <boost/thread.hpp>

#include "Swiften/LinkLocal/DNSSDService.h"
#include "Swiften/EventLoop/EventOwner.h"

namespace Swift {
	class AppleDNSSDService : public DNSSDService, public EventOwner, public boost::enable_shared_from_this<AppleDNSSDService> {
		public:
			AppleDNSSDService();
			~AppleDNSSDService();

			virtual void publishService(const std::map<String,String> properties);
			virtual void start();
			virtual void stop();

		private:
			void doStart();

		private:
			boost::thread* thread;
			bool stopRequested;
	};
}
