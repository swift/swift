#pragma once

#include <boost/signal.hpp>
#include <map>

#include "Swiften/Base/String.h"

namespace Swift {
	class DNSSDService {
		public:
			struct Service {
				Service(const String& name, const String& type, const String& domain, int networkInterface) : name(name), type(type), domain(domain), networkInterface(networkInterface) {}
				String name;
				String type;
				String domain;
				int networkInterface;
			};

			virtual ~DNSSDService();

			virtual void registerService(const String& name, int port, const std::map<String,String>& properties) = 0;
			virtual void unregisterService() = 0;
			virtual void start() = 0;

			boost::signal<void (const Service&)> onServiceAdded;
			boost::signal<void (const Service&)> onServiceRemoved;
			boost::signal<void (const Service&)> onServiceRegistered;
			boost::signal<void ()> onError;
	};
}
