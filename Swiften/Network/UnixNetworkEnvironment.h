/*
 * Copyright (c) 2011 Tobias Markmann
 * Licensed under the simplified BSD license.
 * See Documentation/Licenses/BSD-simplified.txt for more information.
 */

#pragma once

#include <vector>

#include <Swiften/Base/boost_bsignals.h>

#include <Swiften/Network/NetworkEnvironment.h>
#include <Swiften/Network/NetworkInterface.h>

namespace Swift {

class UnixNetworkEnvironment : public NetworkEnvironment {
	class UnixNetworkInterface : public NetworkInterface {
	public:
		typedef boost::shared_ptr<UnixNetworkInterface> ref;

	public:
		UnixNetworkInterface(std::string name) : name(name), loopback(false) { }

		std::vector<HostAddress> getAddresses() {
			return addresses;
		}

		std::string getName() {
			return name;
		}

		bool isLoopback() {
			return loopback;
		}

	private:
		void addHostAddress(HostAddress address) {
			addresses.push_back(address);
		}

		void setLoopback(bool loopback) {
			this->loopback = loopback;
		}

	private:
		friend class UnixNetworkEnvironment;
		std::vector<HostAddress> addresses;
		std::string name;
		InterfaceType type;
		bool loopback;
	};

public:
	std::vector<NetworkInterface::ref> getNetworkInterfaces();
};

}
