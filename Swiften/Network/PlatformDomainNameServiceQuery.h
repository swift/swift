/*
 * Copyright (c) 2010 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

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
			String service;
			bool safeToJoin;
	};
}
