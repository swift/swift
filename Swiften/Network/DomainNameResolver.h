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

		protected:
			static String getNormalized(const String& domain);
	};
}
