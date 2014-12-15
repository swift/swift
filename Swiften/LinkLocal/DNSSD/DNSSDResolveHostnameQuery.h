/*
 * Copyright (c) 2010 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <Swiften/Base/boost_bsignals.h>
#include <boost/optional.hpp>

#include <Swiften/Network/HostAddress.h>

namespace Swift {
	class DNSSDResolveHostnameQuery {
		public:
			virtual ~DNSSDResolveHostnameQuery();

			virtual void run() = 0;
			virtual void finish() = 0;

			boost::signal<void (const boost::optional<HostAddress>&)> onHostnameResolved;
	};
}
