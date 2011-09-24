/*
 * Copyright (c) 2011 Tobias Markmann
 * Licensed under the simplified BSD license.
 * See Documentation/Licenses/BSD-simplified.txt for more information.
 */

#pragma once

#include <Swiften/Network/NATTraversalForwardPortRequest.h>
#include <Swiften/Network/PlatformNATTraversalRequest.h>

namespace Swift {

class NATPMPNATTraversalForwardPortRequest : public NATTraversalForwardPortRequest, public PlatformNATTraversalRequest {
public:
	NATPMPNATTraversalForwardPortRequest(NATTraversalForwardPortRequest::PortMapping, PlatformNATTraversalWorker*);
	virtual ~NATPMPNATTraversalForwardPortRequest();

	virtual void runBlocking();

	virtual void run() {
		doRun();
	}

private:
	NATTraversalForwardPortRequest::PortMapping mapping;
};

}
