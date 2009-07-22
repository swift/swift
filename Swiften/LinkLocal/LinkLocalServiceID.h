#pragma once

#include "Swiften/Base/String.h"

namespace Swift {
	class LinkLocalServiceID {
		public:
			static const String PresenceServiceType;

			LinkLocalServiceID(
				const String& name, 
				const String& type, 
				const String& domain = PresenceServiceType, 
				int networkInterface = 0) : 
					name(name), 
					type(type), 
					domain(domain), 
					networkInterface(networkInterface) {
			}

			bool operator==(const LinkLocalServiceID& o) const {
				return name == o.name && type == o.type && domain == o.domain && (networkInterface != 0 && o.networkInterface != 0 ? networkInterface == o.networkInterface : true);
			}

			bool operator<(const LinkLocalServiceID& o) const {
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

			const String& getName() const {
				return name;
			}

			const String& getType() const {
				return type;
			}

			const String& getDomain() const {
				return domain;
			}

			int getNetworkInterfaceID() const {
				return networkInterface;
			}

		private:
			String name;
			String type;
			String domain;
			int networkInterface;
	};
}
