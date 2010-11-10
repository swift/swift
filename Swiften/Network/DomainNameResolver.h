/*
 * Copyright (c) 2010 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#pragma once

#include <boost/shared_ptr.hpp>

#include "Swiften/Base/String.h"

namespace Swift {
	class DomainNameServiceQuery;
	class DomainNameAddressQuery;
	class String;

	class DomainNameResolver {
		public:
			virtual ~DomainNameResolver();

			virtual boost::shared_ptr<DomainNameServiceQuery> createServiceQuery(const String& name) = 0;
			virtual boost::shared_ptr<DomainNameAddressQuery> createAddressQuery(const String& name) = 0;
	};
}
