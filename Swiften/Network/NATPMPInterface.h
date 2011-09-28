/*
 * Copyright (c) 2011 Remko Tronçon
 * Licensed under the simplified BSD license.
 * See Documentation/Licenses/BSD-simplified.txt for more information.
 */

#pragma once

#include <boost/optional.hpp>
#include <natpmp.h>

#include <Swiften/Network/NATPortMapping.h>
#include <Swiften/Network/NATTraversalInterface.h>

namespace Swift {
	class NATPMPInterface : public NATTraversalInterface {
		public:
			NATPMPInterface();
			~NATPMPInterface();

			virtual bool isAvailable();

			virtual boost::optional<HostAddress> getPublicIP();
			virtual boost::optional<NATPortMapping> addPortForward(int localPort, int publicPort);
			virtual bool removePortForward(const NATPortMapping&);

		private:
			natpmp_t natpmp;
	};
}
