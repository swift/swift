/*
 * Copyright (c) 2010 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
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
