/*
 * Copyright (c) 2010 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <boost/asio/io_service.hpp>
#include <boost/enable_shared_from_this.hpp>

#include <Swiften/Network/DomainNameAddressQuery.h>
#include <Swiften/Network/PlatformDomainNameQuery.h>
#include <Swiften/EventLoop/EventOwner.h>
#include <string>

namespace Swift {
	class PlatformDomainNameResolver;
	class EventLoop;

	class PlatformDomainNameAddressQuery : public DomainNameAddressQuery, public PlatformDomainNameQuery, public boost::enable_shared_from_this<PlatformDomainNameAddressQuery>, public EventOwner {
		public:
			PlatformDomainNameAddressQuery(const boost::optional<std::string>& host, EventLoop* eventLoop, PlatformDomainNameResolver*);

			void run();

		private:
			void runBlocking();
			void emitError();

		private:
			boost::asio::io_service ioService;
			std::string hostname;
			bool hostnameValid;
			EventLoop* eventLoop;
	};
}


