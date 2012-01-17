/*
 * Copyright (c) 2012 Kevin Smith
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#pragma once

#include <boost/shared_ptr.hpp>

#include <Swiften/Network/DomainNameResolver.h>
#include <Swiften/Network/StaticDomainNameResolver.h>


namespace Swift {
	class EventLoop;
	class CachingNameOnlyDomainNameResolver : public DomainNameResolver {
		public:
			CachingNameOnlyDomainNameResolver(DomainNameResolver* realResolver, EventLoop* eventLoop);
			~CachingNameOnlyDomainNameResolver();

			virtual DomainNameServiceQuery::ref createServiceQuery(const std::string& name);
			virtual DomainNameAddressQuery::ref createAddressQuery(const std::string& name);

		private:
			void handleAddressQueryResult(const std::string hostname, const std::vector<HostAddress>& address, boost::optional<DomainNameResolveError> error);

		private:
			DomainNameResolver* realResolver;
			boost::shared_ptr<StaticDomainNameResolver> staticResolver;
	};
}
