#pragma once

#include <boost/signal.hpp>
#include <map>

#include "Swiften/Base/String.h"

namespace Swift {
	class DNSSDService {
		public:
			struct DiscoveredService {
				String name;
				String type;
				String domain;
				int networkInterface;
			};

			virtual ~DNSSDService();

			virtual void publishService(const std::map<String,String> properties) = 0;
			virtual void start() = 0;

			boost::signal<void (const DiscoveredService&)> onServiceAdded;
			boost::signal<void (const DiscoveredService&)> onServiceRemoved;
			boost::signal<void ()> onServiceRegistered;
	};
}
