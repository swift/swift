/*
 * Copyright (c) 2011 Tobias Markmann
 * Licensed under the simplified BSD license.
 * See Documentation/Licenses/BSD-simplified.txt for more information.
 */

#pragma once

#include <Swiften/Network/PlatformNATTraversalRequest.h>
#include <Swiften/Network/NATTraversalRemovePortForwardingRequest.h>

namespace Swift {

class UPnPNATTraversalRemovePortForwardingRequest : public NATTraversalRemovePortForwardingRequest, public PlatformNATTraversalRequest {
public:
	UPnPNATTraversalRemovePortForwardingRequest(NATTraversalRemovePortForwardingRequest::PortMapping, PlatformNATTraversalWorker*);
	virtual ~UPnPNATTraversalRemovePortForwardingRequest();

	virtual void runBlocking();

	virtual void run() {
		doRun();
	}

private:
	NATTraversalRemovePortForwardingRequest::PortMapping mapping;
};

}
