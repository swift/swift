#pragma once

#include <boost/signal.hpp>

#include "Swiften/LinkLocal/LinkLocalServiceID.h"

namespace Swift {
	class DNSSDBrowseQuery {
		public:
			virtual ~DNSSDBrowseQuery();

			virtual void startBrowsing() = 0;
			virtual void stopBrowsing() = 0;

			boost::signal<void (const LinkLocalServiceID&)> onServiceAdded;
			boost::signal<void (const LinkLocalServiceID&)> onServiceRemoved;
	};
}
