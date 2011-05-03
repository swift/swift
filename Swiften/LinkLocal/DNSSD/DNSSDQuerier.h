/*
 * Copyright (c) 2010 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#pragma once

#include <boost/shared_ptr.hpp>

#include <Swiften/Base/ByteArray.h>

namespace Swift {
	class DNSSDServiceID;
	class DNSSDBrowseQuery;
	class DNSSDRegisterQuery;
	class DNSSDResolveServiceQuery;
	class DNSSDResolveHostnameQuery;

	class DNSSDQuerier {
		public:
			virtual ~DNSSDQuerier();

			virtual void start() = 0;
			virtual void stop() = 0;

			virtual boost::shared_ptr<DNSSDBrowseQuery> createBrowseQuery() = 0;
			virtual boost::shared_ptr<DNSSDRegisterQuery> createRegisterQuery(
					const std::string& name, int port, const ByteArray& info) = 0;
			virtual boost::shared_ptr<DNSSDResolveServiceQuery> createResolveServiceQuery(
					const DNSSDServiceID&) = 0;
			virtual boost::shared_ptr<DNSSDResolveHostnameQuery> createResolveHostnameQuery(
					const std::string& hostname, int interfaceIndex) = 0;
	};
}
