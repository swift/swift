/*
 * Copyright (c) 2010 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#pragma once

#include <boost/enable_shared_from_this.hpp>

#include "Swiften/Network/DomainNameServiceQuery.h"
#include "Swiften/EventLoop/EventOwner.h"
#include "Swiften/Base/String.h"
#include <Swiften/Network/PlatformDomainNameQuery.h>

namespace Swift {
	class EventLoop;

	class PlatformDomainNameServiceQuery : public DomainNameServiceQuery, public PlatformDomainNameQuery, public boost::enable_shared_from_this<PlatformDomainNameServiceQuery>, public EventOwner {
		public:
			PlatformDomainNameServiceQuery(const String& service, EventLoop* eventLoop, PlatformDomainNameResolver* resolver);

			virtual void run();

		private:
			void runBlocking();
			void emitError();

		private:
			EventLoop* eventLoop;
			String service;
	};
}
