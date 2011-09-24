/*
 * Copyright (c) 2011 Tobias Markmann
 * Licensed under the simplified BSD license.
 * See Documentation/Licenses/BSD-simplified.txt for more information.
 */

#pragma once

#include <Swiften/Network/PlatformNATTraversalRequest.h>
#include <Swiften/Network/NATTraversalGetPublicIPRequest.h>

namespace Swift {

class NATPMPNATTraversalGetPublicIPRequest : public NATTraversalGetPublicIPRequest, public PlatformNATTraversalRequest {
public:
	NATPMPNATTraversalGetPublicIPRequest(PlatformNATTraversalWorker*);
	virtual ~NATPMPNATTraversalGetPublicIPRequest();

	virtual void runBlocking();

	virtual void run() {
		doRun();
	}
};

}
