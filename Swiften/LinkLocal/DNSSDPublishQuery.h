#pragma once

#include <boost/signal.hpp>
#include <boost/optional.hpp>

#include "Swiften/LinkLocal/LinkLocalServiceID.h"

namespace Swift {
	class DNSSDPublishQuery {
		public:
			virtual ~DNSSDPublishQuery();

			virtual void publish() = 0;

			boost::signal<void (boost::optional<LinkLocalServiceID>)> onPublishFinished;
	};
}
