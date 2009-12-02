#pragma once

#include <boost/shared_ptr.hpp>

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
