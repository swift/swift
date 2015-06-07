/*
 * Copyright (c) 2011 Tobias Markmann
 * Licensed under the simplified BSD license.
 * See Documentation/Licenses/BSD-simplified.txt for more information.
 */

/*
 * Copyright (c) 2015 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <Swiften/Base/API.h>
#include <Swiften/Network/HostAddress.h>

namespace Swift {
	class SWIFTEN_API NATPortMapping {
	public:
		enum Protocol {
			TCP,
			UDP
		};

		NATPortMapping(int localPort, int publicPort, Protocol protocol = TCP, int leaseInSeconds = 60 * 60 * 24) : 
			publicPort(publicPort), localPort(localPort), protocol(protocol), leaseInSeconds(leaseInSeconds) {

		}

		int getPublicPort() const {
			return publicPort;
		}

		int getLocalPort() const {
			return localPort;
		}

		Protocol getProtocol() const {
			return protocol;
		}

		int getLeaseInSeconds() const {
			return leaseInSeconds;
		}

	private:
		int publicPort;
		int localPort;
		Protocol protocol;
		int leaseInSeconds;
	};
}
