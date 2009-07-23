#pragma once

#include <boost/shared_ptr.hpp>

namespace Swift {
	class DNSSDService;

	class DNSSDServiceFactory {
		public:
			virtual ~DNSSDServiceFactory();
			virtual bool canCreate() const = 0;

			virtual boost::shared_ptr<DNSSDService> createDNSSDService() = 0;
	};
}
