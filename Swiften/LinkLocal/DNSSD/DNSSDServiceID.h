/*
 * Copyright (c) 2010 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#pragma once

#include <string>

namespace Swift {
	class DNSSDServiceID {
		public:
			static const char* PresenceServiceType;

			DNSSDServiceID(
				const std::string& name, 
				const std::string& domain, 
				const std::string& type = PresenceServiceType, 
				int networkInterface = -1) : 
					name(name), 
					domain(domain), 
					type(type), 
					networkInterface(networkInterface) {
			}

			bool operator==(const DNSSDServiceID& o) const {
				return name == o.name && domain == o.domain && type == o.type && (networkInterface != 0 && o.networkInterface != 0 ? networkInterface == o.networkInterface : true);
			}

			bool operator<(const DNSSDServiceID& o) const {
				if (o.name == name) {
					if (o.domain == domain) {
						if (o.type == type) {
							return networkInterface < o.networkInterface;
						}
						else {
							return type < o.type;
						}
					}
					else {
						return domain < o.domain;
					}
				}
				else {
					return o.name < name;
				}
			}

			const std::string& getName() const {
				return name;
			}

			const std::string& getDomain() const {
				return domain;
			}

			const std::string& getType() const {
				return type;
			}

			int getNetworkInterfaceID() const {
				return networkInterface;
			}

		private:
			std::string name;
			std::string domain;
			std::string type;
			int networkInterface;
	};
}
