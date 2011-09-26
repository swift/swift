/*
 * Copyright (c) 2011 Tobias Markmann
 * Licensed under the simplified BSD license.
 * See Documentation/Licenses/BSD-simplified.txt for more information.
 */

#pragma once

#include <Swiften/Network/NATTraversalForwardPortRequest.h>
#include <Swiften/Network/PlatformNATTraversalRequest.h>

namespace Swift {

class UPnPNATTraversalForwardPortRequest : public NATTraversalForwardPortRequest, public PlatformNATTraversalRequest {
public:
	UPnPNATTraversalForwardPortRequest(NATTraversalForwardPortRequest::PortMapping, PlatformNATTraversalWorker*);
	virtual ~UPnPNATTraversalForwardPortRequest();

	virtual void runBlocking();

	virtual void run() {
		doRun();
	}

private:
	NATTraversalForwardPortRequest::PortMapping mapping;
};

}
