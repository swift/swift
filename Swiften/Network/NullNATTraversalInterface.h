/*
 * Copyright (c) 2011 Remko Tronçon
 * Licensed under the simplified BSD license.
 * See Documentation/Licenses/BSD-simplified.txt for more information.
 */

#pragma once

#include <boost/optional.hpp>

#include <Swiften/Network/NATTraversalInterface.h>

namespace Swift {
	class NullNATTraversalInterface : public NATTraversalInterface {
		public:
			virtual bool isAvailable() {
				return true;
			}

			virtual boost::optional<HostAddress> getPublicIP() {
				return boost::optional<HostAddress>();
			}

			virtual boost::optional<NATPortMapping> addPortForward(int, int) {
				return boost::optional<NATPortMapping>();
			}

			virtual bool removePortForward(const NATPortMapping&) {
				return false;
			}
	};
}
