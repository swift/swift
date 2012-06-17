/*
 * Copyright (c) 2012 Kevin Smith
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#pragma once

#include <boost/shared_ptr.hpp>

#include <Swiften/Network/DomainNameResolver.h>
#include <Swiften/Network/StaticDomainNameResolver.h>

/*
 * FIXME: Does not do any caching yet.
 */
namespace Swift {
	class EventLoop;

	class CachingDomainNameResolver : public DomainNameResolver {
		public:
			CachingDomainNameResolver(DomainNameResolver* realResolver, EventLoop* eventLoop);
			~CachingDomainNameResolver();

			virtual DomainNameServiceQuery::ref createServiceQuery(const std::string& name);
			virtual DomainNameAddressQuery::ref createAddressQuery(const std::string& name);

		private:
			DomainNameResolver* realResolver;
	};
}
