/*
 * Copyright (c) 2011 Remko Tronçon
 * Licensed under the simplified BSD license.
 * See Documentation/Licenses/BSD-simplified.txt for more information.
 */

#pragma once

#include <boost/optional.hpp>
#include <miniupnpc.h>

#include <Swiften/Network/NATPortMapping.h>
#include <Swiften/Network/NATTraversalInterface.h>

namespace Swift {
	class MiniUPnPInterface : public NATTraversalInterface {
		public:
			MiniUPnPInterface();
			~MiniUPnPInterface();

			virtual bool isAvailable() {
				return isValid;
			}

			boost::optional<HostAddress> getPublicIP();
			boost::optional<NATPortMapping> addPortForward(int localPort, int publicPort);
			bool removePortForward(const NATPortMapping&);

		private:
			bool isValid;
			std::string localAddress;
			UPNPDev* deviceList;
			UPNPUrls urls;
			IGDdatas data;
	};
}
