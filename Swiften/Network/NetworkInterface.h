/*
 * Copyright (c) 2011 Tobias Markmann
 * Licensed under the simplified BSD license.
 * See Documentation/Licenses/BSD-simplified.txt for more information.
 */

#pragma once

#include <vector>

#include <Swiften/Network/HostAddress.h>

namespace Swift {
	class NetworkInterface {
		public:
			enum Type {
				Unknown,
				WLAN,
				Ethernet,
				Mobile,
				VPN,
			};

		public:
			NetworkInterface(const std::string& name, bool loopback, Type type = Unknown) : name(name), loopback(loopback), type(type) {
			}

			void addAddress(const HostAddress& address) {
				addresses.push_back(address);
			}

			const std::vector<HostAddress>& getAddresses() const {
				return addresses;
			}

			const std::string& getName() const {
				return name;
			}

			bool isLoopback() const {
				return loopback;
			}

		private:
			std::string name;
			bool loopback;
			Type type;
			std::vector<HostAddress> addresses;
	};
}
