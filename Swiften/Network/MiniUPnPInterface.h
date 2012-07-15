/*
 * Copyright (c) 2011 Remko Tronçon
 * Licensed under the simplified BSD license.
 * See Documentation/Licenses/BSD-simplified.txt for more information.
 */

#pragma once

#include <boost/optional.hpp>
#include <boost/noncopyable.hpp>
#include <boost/shared_ptr.hpp>

#include <Swiften/Network/NATPortMapping.h>
#include <Swiften/Network/NATTraversalInterface.h>

namespace Swift {
	class MiniUPnPInterface : public NATTraversalInterface, boost::noncopyable {
		public:
			MiniUPnPInterface();
			~MiniUPnPInterface();

			virtual bool isAvailable();

			boost::optional<HostAddress> getPublicIP();
			boost::optional<NATPortMapping> addPortForward(int localPort, int publicPort);
			bool removePortForward(const NATPortMapping&);

		private:
			struct Private;
			boost::shared_ptr<Private> p;
	};
}
