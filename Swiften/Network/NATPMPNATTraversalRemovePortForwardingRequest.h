/*
 * Copyright (c) 2011 Tobias Markmann
 * Licensed under the simplified BSD license.
 * See Documentation/Licenses/BSD-simplified.txt for more information.
 */

#pragma once

#include <Swiften/Network/NATTraversalRemovePortForwardingRequest.h>
#include <Swiften/Network/PlatformNATTraversalRequest.h>

namespace Swift {

class NATPMPNATTraversalRemovePortForwardingRequest : public NATTraversalRemovePortForwardingRequest, public PlatformNATTraversalRequest {
public:
	NATPMPNATTraversalRemovePortForwardingRequest(PortMapping, PlatformNATTraversalWorker*);
	virtual ~NATPMPNATTraversalRemovePortForwardingRequest();

	virtual void runBlocking();

	virtual void run() {
		doRun();
	}

private:
	HostAddress getLocalClient();

private:
	PortMapping mapping;
};

}
