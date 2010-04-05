#pragma once

#include <boost/signals.hpp>
#include <boost/optional.hpp>
#include <boost/shared_ptr.hpp>

#include "Swiften/Network/DomainNameResolveError.h"
#include "Swiften/Network/HostAddress.h"

namespace Swift {
	class DomainNameAddressQuery {
		public:
			virtual ~DomainNameAddressQuery();

			virtual void run() = 0;

			boost::signal<void (const std::vector<HostAddress>&, boost::optional<DomainNameResolveError>)> onResult;
	};
}
