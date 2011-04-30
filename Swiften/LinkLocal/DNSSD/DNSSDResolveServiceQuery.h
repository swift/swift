/*
 * Copyright (c) 2010 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#pragma once

#include <Swiften/Base/boost_bsignals.h>
#include <boost/optional.hpp>

#include <Swiften/LinkLocal/DNSSD/DNSSDServiceID.h>
#include <Swiften/Base/ByteArray.h>

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
