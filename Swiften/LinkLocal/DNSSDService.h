#pragma once

#include <boost/signal.hpp>
#include <map>

#include "Swiften/Base/String.h"
#include "Swiften/LinkLocal/LinkLocalServiceInfo.h"

namespace Swift {
	class LinkLocalServiceInfo;

	class DNSSDService {
		public:
			struct Service {
				Service(const String& name, const String& type, const String& domain, int networkInterface) : name(name), type(type), domain(domain), networkInterface(networkInterface) {}
				bool operator==(const Service& o) const {
					return name == o.name && type == o.type && domain == o.domain && (networkInterface != 0 && o.networkInterface != 0 ? networkInterface == o.networkInterface : true);
				}
				bool operator<(const Service& o) const {
					if (o.name == name) {
						if (o.type == type) {
							if (o.domain == domain) {
								return networkInterface < o.networkInterface;
							}
							else {
								return domain < o.domain;
							}
						}
						else {
							return type < o.type;
						}
					}
					else {
						return o.name < name;
					}
				}

				String name;
				String type;
				String domain;
				int networkInterface;
			};

			struct ResolveResult {
				ResolveResult(const String& host, int port, const LinkLocalServiceInfo& info) : host(host), port(port), info(info) {}
				String host;
				int port;
				LinkLocalServiceInfo info;
			};

			virtual ~DNSSDService();

			virtual void registerService(const String& name, int port, const LinkLocalServiceInfo&) = 0;
			virtual void startResolvingService(const Service&) = 0;
			virtual void stopResolvingService(const Service&) = 0;
			virtual void unregisterService() = 0;
			virtual void start() = 0;

			boost::signal<void (const Service&)> onServiceAdded;
			boost::signal<void (const Service&)> onServiceRemoved;
			boost::signal<void (const Service&)> onServiceRegistered;
			boost::signal<void (const Service&, const ResolveResult&)> onServiceResolved;
			boost::signal<void ()> onError;
	};
}
