#pragma once

#include <vector>
#include <map>

#include "Swiften/Network/HostAddress.h"
#include "Swiften/Network/HostAddressPort.h"
#include "Swiften/Network/DomainNameResolver.h"
#include "Swiften/Network/DomainNameServiceQuery.h"
#include "Swiften/Network/DomainNameAddressQuery.h"

namespace Swift {
	class String;

	class StaticDomainNameResolver : public DomainNameResolver {
		public:
			typedef std::map<String, HostAddress> AddressesMap;
			typedef std::vector< std::pair<String, DomainNameServiceQuery::Result> > ServicesCollection;

		public:
			void addAddress(const String& domain, const HostAddress& address);
			void addService(const String& service, const DomainNameServiceQuery::Result& result);
			void addXMPPClientService(const String& domain, const HostAddressPort&);

			const AddressesMap& getAddresses() const {
				return addresses;
			}

			const ServicesCollection& getServices() const {
				return services;
			}

			virtual boost::shared_ptr<DomainNameServiceQuery> createServiceQuery(const String& name);
			virtual boost::shared_ptr<DomainNameAddressQuery> createAddressQuery(const String& name);
			
		private:
			AddressesMap addresses;
			ServicesCollection services;
	};
}
