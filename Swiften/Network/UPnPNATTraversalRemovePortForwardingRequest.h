/*
 * Copyright (c) 2011 Tobias Markmann
 * Licensed under the simplified BSD license.
 * See Documentation/Licenses/BSD-simplified.txt for more information.
 */

#pragma once

#include <Swiften/Network/PlatformNATTraversalRemovePortForwardingRequest.h>

namespace Swift {

class UPnPNATTraversalRemovePortForwardingRequest : public PlatformNATTraversalRemovePortForwardingRequest {
public:
	UPnPNATTraversalRemovePortForwardingRequest(PlatformNATTraversalRemovePortForwardingRequest::PortMapping, PlatformNATTraversalWorker*);
	virtual ~UPnPNATTraversalRemovePortForwardingRequest();

	virtual void runBlocking();

private:
	HostAddress getLocalClient();

private:
	PlatformNATTraversalRemovePortForwardingRequest::PortMapping mapping;
};

}
