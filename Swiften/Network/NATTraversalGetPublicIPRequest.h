/*
 * Copyright (c) 2011 Tobias Markmann
 * Licensed under the simplified BSD license.
 * See Documentation/Licenses/BSD-simplified.txt for more information.
 */

#pragma once

#include <Swiften/Base/boost_bsignals.h>
#include <Swiften/Network/HostAddress.h>

namespace Swift {
	class NATTraversalGetPublicIPRequest {
		public:
			virtual ~NATTraversalGetPublicIPRequest();

			virtual void run() = 0;

			boost::signal<void (boost::optional<HostAddress>)> onResult;
	};
}
