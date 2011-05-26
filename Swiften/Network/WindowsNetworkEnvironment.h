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

class WindowsNetworkEnvironment : public NetworkEnvironment {
	class WindowsNetworkInterface : public NetworkInterface {
	public:
		typedef boost::shared_ptr<WindowsNetworkInterface> ref;

	public:
		virtual ~WindowsNetworkInterface() { }
		virtual std::vector<HostAddress> getAddresses() {
			return addresses;
		}

		virtual std::string getName() {
			return name;
		}

		virtual bool isLoopback() {
			return false;
		}

	public:
		void addHostAddress(HostAddress address) {
			addresses.push_back(address);
		}

		void setName(const std::string& name) {
			this->name = name;
		}

	private:
		std::vector<HostAddress> addresses;
		InterfaceType type;
		std::string name;
	};

public:
	std::vector<NetworkInterface::ref> getNetworkInterfaces();
};

}
