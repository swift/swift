#pragma once

#include <boost/signal.hpp>

#include "Swiften/LinkLocal/LinkLocalServiceID.h"

namespace Swift {
	class DNSSDBrowseQuery {
		public:
			virtual ~DNSSDBrowseQuery();

			boost::signal<void (const LinkLocalServiceID&)> onServiceAdded;
			boost::signal<void (const LinkLocalServiceID&)> onServiceRemoved;
	};
}
