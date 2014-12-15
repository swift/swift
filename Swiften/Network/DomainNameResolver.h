/*
 * Copyright (c) 2010 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <boost/shared_ptr.hpp>
#include <string>

#include <Swiften/Base/API.h>

namespace Swift {
	class DomainNameServiceQuery;
	class DomainNameAddressQuery;
	

	class SWIFTEN_API DomainNameResolver {
		public:
			virtual ~DomainNameResolver();

			virtual boost::shared_ptr<DomainNameServiceQuery> createServiceQuery(const std::string& serviceLookupPrefix, const std::string& domain) = 0;
			virtual boost::shared_ptr<DomainNameAddressQuery> createAddressQuery(const std::string& name) = 0;
	};
}
