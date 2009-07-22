#pragma once

#include <boost/signal.hpp>
#include <map>

#include "Swiften/Base/String.h"
#include "Swiften/LinkLocal/LinkLocalServiceID.h"
#include "Swiften/LinkLocal/LinkLocalServiceInfo.h"

namespace Swift {
	class LinkLocalServiceInfo;
	class HostAddress;

	class DNSSDService {
		public:
			struct ResolveResult {
				ResolveResult(const String& host, int port, const LinkLocalServiceInfo& info) : host(host), port(port), info(info) {}
				String host;
				int port;
				LinkLocalServiceInfo info;
			};

			virtual ~DNSSDService();

			virtual void start() = 0;
			virtual void stop() = 0;

			virtual void registerService(const String& name, int port, const LinkLocalServiceInfo&) = 0;
			virtual void updateService(const LinkLocalServiceInfo&) = 0;
			virtual void unregisterService() = 0;

			virtual void startResolvingService(const LinkLocalServiceID&) = 0;
			virtual void stopResolvingService(const LinkLocalServiceID&) = 0;
			
			virtual void resolveHostname(const String& hostname, int interfaceIndex = 0) = 0;

			boost::signal<void ()> onStarted;
			boost::signal<void (bool)> onStopped;
			boost::signal<void (const LinkLocalServiceID&)> onServiceAdded;
			boost::signal<void (const LinkLocalServiceID&)> onServiceRemoved;
			boost::signal<void (const LinkLocalServiceID&)> onServiceRegistered;
			boost::signal<void (const LinkLocalServiceID&, const ResolveResult&)> onServiceResolved;
			boost::signal<void (const String&, const boost::optional<HostAddress>&)> onHostnameResolved;
	};
}
