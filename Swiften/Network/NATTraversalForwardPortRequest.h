/*
 * Copyright (c) 2011 Tobias Markmann
 * Licensed under the simplified BSD license.
 * See Documentation/Licenses/BSD-simplified.txt for more information.
 */

#pragma once

#include <Swiften/Base/boost_bsignals.h>

#include <Swiften/Network/NATPortMapping.h>

namespace Swift {
	class NATTraversalForwardPortRequest {
		public:
			virtual ~NATTraversalForwardPortRequest();

			virtual void run() = 0;

			boost::signal<void (boost::optional<NATPortMapping>)> onResult;
	};
}
