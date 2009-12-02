#pragma once

#include <boost/thread.hpp>
#include <boost/enable_shared_from_this.hpp>

#include "Swiften/Network/DomainNameServiceQuery.h"
#include "Swiften/EventLoop/EventOwner.h"
#include "Swiften/Base/String.h"

namespace Swift {
	class PlatformDomainNameServiceQuery : public DomainNameServiceQuery, public boost::enable_shared_from_this<PlatformDomainNameServiceQuery>, public EventOwner {
		public:
			PlatformDomainNameServiceQuery(const String& service);
			~PlatformDomainNameServiceQuery();

			virtual void run();

		private:
			void doRun();
			void emitError();

		private:
			boost::thread* thread;
			bool safeToJoin;
			String service;
	};
}
