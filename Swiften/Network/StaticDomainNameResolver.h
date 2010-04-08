/*
 * Copyright (c) 2010 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#pragma once

#include <vector>
#include <map>

#include "Swiften/Network/HostAddress.h"
#include "Swiften/Network/HostAddressPort.h"
#include "Swiften/Network/DomainNameResolver.h"
#include "Swiften/Network/DomainNameServiceQuery.h"
#include "Swiften/Network/DomainNameAddressQuery.h"
#include "Swiften/EventLoop/MainEventLoop.h"

namespace Swift {
	class String;

	class StaticDomainNameResolver : public DomainNameResolver {
		public:
			typedef std::map<String, std::vector<HostAddress> > AddressesMap;
			typedef std::vector< std::pair<String, DomainNameServiceQuery::Result> > ServicesCollection;

		public:
			StaticDomainNameResolver();

			void addAddress(const String& domain, const HostAddress& address);
			void addService(const String& service, const DomainNameServiceQuery::Result& result);
			void addXMPPClientService(const String& domain, const HostAddressPort&);
			void addXMPPClientService(const String& domain, const String& host, int port);

			const AddressesMap& getAddresses() const {
				return addresses;
			}

			const ServicesCollection& getServices() const {
				return services;
			}

			bool getIsResponsive() const {
				return isResponsive;
			}

			void setIsResponsive(bool b) {
				isResponsive = b;
			}

			virtual boost::shared_ptr<DomainNameServiceQuery> createServiceQuery(const String& name);
			virtual boost::shared_ptr<DomainNameAddressQuery> createAddressQuery(const String& name);
			
		private:
			bool isResponsive;
			AddressesMap addresses;
			ServicesCollection services;
	};
}
