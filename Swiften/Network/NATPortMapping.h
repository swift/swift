/*
 * Copyright (c) 2011 Tobias Markmann
 * Licensed under the simplified BSD license.
 * See Documentation/Licenses/BSD-simplified.txt for more information.
 */

#pragma once

#include <Swiften/Network/HostAddress.h>

namespace Swift {
	struct NATPortMapping {
		enum Protocol {
			TCP,
			UDP,
		};

		NATPortMapping(int localPort, int publicPort, Protocol protocol = TCP, int leaseInSeconds = 60 * 60 * 24) : publicPort(publicPort), localPort(localPort), protocol(protocol), leaseInSeconds(leaseInSeconds) {

		}

		int publicPort;
		int localPort;
		Protocol protocol;
		int leaseInSeconds;
	};
}
