/*
 * Copyright (c) 2010 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#pragma once

#include <boost/shared_ptr.hpp>

namespace Swift {
	class DNSSDQuerier;

	class PlatformDNSSDQuerierFactory {
		public:
			 boost::shared_ptr<DNSSDQuerier> createQuerier();

			 bool canCreate();
	};
}
