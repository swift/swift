/*
 * Copyright (c) 2011 Tobias Markmann
 * Licensed under the simplified BSD license.
 * See Documentation/Licenses/BSD-simplified.txt for more information.
 */

#pragma once

#include <Swiften/Base/boost_bsignals.h>
#include <Swiften/Network/HostAddress.h>
#include <Swiften/Network/PlatformNATTraversalRequest.h>

namespace Swift {

class PlatformNATTraversalWorker;

class PlatformNATTraversalRemovePortForwardingRequest : public PlatformNATTraversalRequest {
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
	PlatformNATTraversalRemovePortForwardingRequest(PlatformNATTraversalWorker* worker);
	virtual ~PlatformNATTraversalRemovePortForwardingRequest();

	boost::signal<void (boost::optional<bool> /* failure */)> onResult;
};

}
