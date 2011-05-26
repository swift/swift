/*
 * Copyright (c) 2011 Tobias Markmann
 * Licensed under the simplified BSD license.
 * See Documentation/Licenses/BSD-simplified.txt for more information.
 */

#pragma once

#include <Swiften/Base/boost_bsignals.h>
#include <Swiften/Network/HostAddress.h>
#include <Swiften/Network/PlatformNATTraversalRequest.h>

namespace Swift {

class PlatformNATTraversalWorker;

class PlatformNATTraversalGetPublicIPRequest : public PlatformNATTraversalRequest {
public:
	PlatformNATTraversalGetPublicIPRequest(PlatformNATTraversalWorker* worker);
	virtual ~PlatformNATTraversalGetPublicIPRequest();

	boost::signal<void (boost::optional<HostAddress>)> onResult;
};

}
