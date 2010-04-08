/*
 * Copyright (c) 2010 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#pragma once

#include "Swiften/Network/DomainNameResolver.h"

namespace Swift {
	class String;

	class PlatformDomainNameResolver : public DomainNameResolver {
		public:
			PlatformDomainNameResolver();

			virtual boost::shared_ptr<DomainNameServiceQuery> createServiceQuery(const String& name);
			virtual boost::shared_ptr<DomainNameAddressQuery> createAddressQuery(const String& name);
	};
}
