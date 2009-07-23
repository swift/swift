#pragma once

#include "Swiften/LinkLocal/DNSSDServiceFactory.h"

namespace Swift {
	class PlatformDNSSDServiceFactory : public DNSSDServiceFactory {
		public:
			PlatformDNSSDServiceFactory();

			bool canCreate() const;
			virtual boost::shared_ptr<DNSSDService> createDNSSDService();
	};
}
