/*
 * Copyright (c) 2011 Tobias Markmann
 * Licensed under the simplified BSD license.
 * See Documentation/Licenses/BSD-simplified.txt for more information.
 */

#pragma once

#include <Swiften/Base/boost_bsignals.h>
#include <Swiften/Network/HostAddress.h>

namespace Swift {
	class NATTraversalRemovePortForwardingRequest {
		public:
			struct PortMapping {
				enum Protocol {
					TCP,
					UDP,
				};

				unsigned int publicPort;
				unsigned int localPort;
				Protocol protocol;
				unsigned long leaseInSeconds;
			};

		public:
			virtual ~NATTraversalRemovePortForwardingRequest();

			virtual void run() = 0;

			boost::signal<void (boost::optional<bool> /* failure */)> onResult;
	};
}
