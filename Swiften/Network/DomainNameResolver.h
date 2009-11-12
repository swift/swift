#pragma once

#include <vector>

#include "Swiften/Network/HostAddressPort.h"

namespace Swift {
	class String;

	class DomainNameResolver {
		public:
			virtual ~DomainNameResolver();

			virtual std::vector<HostAddressPort> resolve(const String& domain) = 0;
	};
}
