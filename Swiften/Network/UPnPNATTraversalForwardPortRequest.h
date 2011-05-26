/*
 * Copyright (c) 2011 Tobias Markmann
 * Licensed under the simplified BSD license.
 * See Documentation/Licenses/BSD-simplified.txt for more information.
 */

#pragma once

#include <Swiften/Network/PlatformNATTraversalForwardPortRequest.h>

namespace Swift {

class UPnPNATTraversalForwardPortRequest : public PlatformNATTraversalForwardPortRequest {
public:
	UPnPNATTraversalForwardPortRequest(PlatformNATTraversalForwardPortRequest::PortMapping, PlatformNATTraversalWorker*);
	virtual ~UPnPNATTraversalForwardPortRequest();

	virtual void runBlocking();

private:
	HostAddress getLocalClient();

private:
	PlatformNATTraversalForwardPortRequest::PortMapping mapping;
};

}
