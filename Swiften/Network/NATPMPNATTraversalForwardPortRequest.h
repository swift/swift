/*
 * Copyright (c) 2011 Tobias Markmann
 * Licensed under the simplified BSD license.
 * See Documentation/Licenses/BSD-simplified.txt for more information.
 */

#pragma once

#include <Swiften/Network/PlatformNATTraversalForwardPortRequest.h>

namespace Swift {

class NATPMPNATTraversalForwardPortRequest : public PlatformNATTraversalForwardPortRequest {
public:
	NATPMPNATTraversalForwardPortRequest(PlatformNATTraversalForwardPortRequest::PortMapping, PlatformNATTraversalWorker*);
	virtual ~NATPMPNATTraversalForwardPortRequest();

	virtual void runBlocking();

private:
	PlatformNATTraversalForwardPortRequest::PortMapping mapping;
};

}
