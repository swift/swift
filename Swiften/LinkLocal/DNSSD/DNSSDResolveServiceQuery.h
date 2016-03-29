/*
 * Copyright (c) 2010-2016 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <boost/optional.hpp>

#include <Swiften/Base/ByteArray.h>
#include <Swiften/Base/boost_bsignals.h>
#include <Swiften/LinkLocal/DNSSD/DNSSDServiceID.h>

namespace Swift {
	class DNSSDResolveServiceQuery {
		public:
			struct Result {
				Result(const std::string& fullName, const std::string& host, int port, const ByteArray& info) : 
							fullName(fullName), host(host), port(port), info(info) {}
				std::string fullName;
				std::string host;
				int port;
				ByteArray info;
			};

			virtual ~DNSSDResolveServiceQuery();

			virtual void start() = 0;
			virtual void stop() = 0;

			boost::signal<void (const boost::optional<Result>&)> onServiceResolved;
	};
}
