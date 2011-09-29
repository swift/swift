/*
 * Copyright (c) 2011 Remko Tronçon
 * Licensed under the simplified BSD license.
 * See Documentation/Licenses/BSD-simplified.txt for more information.
 */

#pragma once

#include <boost/optional.hpp>

#include <Swiften/Network/NATPortMapping.h>

namespace Swift {
	class NATTraversalInterface {
		public:
			virtual ~NATTraversalInterface();

			virtual bool isAvailable() = 0;

			virtual boost::optional<HostAddress> getPublicIP() = 0;
			virtual boost::optional<NATPortMapping> addPortForward(int localPort, int publicPort) = 0;
			virtual bool removePortForward(const NATPortMapping&) = 0;
	};
}
