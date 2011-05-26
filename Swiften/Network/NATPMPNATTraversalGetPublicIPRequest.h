/*
 * Copyright (c) 2011 Tobias Markmann
 * Licensed under the simplified BSD license.
 * See Documentation/Licenses/BSD-simplified.txt for more information.
 */

#pragma once

#include <Swiften/Network/PlatformNATTraversalGetPublicIPRequest.h>

namespace Swift {

class NATPMPNATTraversalGetPublicIPRequest : public PlatformNATTraversalGetPublicIPRequest {
public:
	NATPMPNATTraversalGetPublicIPRequest(PlatformNATTraversalWorker*);
	virtual ~NATPMPNATTraversalGetPublicIPRequest();

	//virtual void run();
	virtual void runBlocking();
};

}
