#pragma once

#include <boost/shared_ptr.hpp>

namespace Swift {
	class DNSSDQuerier;

	class PlatformDNSSDQuerierFactory {
		public:
			 boost::shared_ptr<DNSSDQuerier> createQuerier();
	};
}
