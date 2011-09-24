/*
 * Copyright (c) 2011 Tobias Markmann
 * Licensed under the simplified BSD license.
 * See Documentation/Licenses/BSD-simplified.txt for more information.
 */

#pragma once

#include <Swiften/Network/NATTraversalGetPublicIPRequest.h>
#include <Swiften/Network/PlatformNATTraversalRequest.h>

namespace Swift {

class UPnPNATTraversalGetPublicIPRequest : public NATTraversalGetPublicIPRequest, public PlatformNATTraversalRequest {
public:
	UPnPNATTraversalGetPublicIPRequest(PlatformNATTraversalWorker*);
	virtual ~UPnPNATTraversalGetPublicIPRequest();

	virtual void runBlocking();

	virtual void run() {
		doRun();
	}
};

}
