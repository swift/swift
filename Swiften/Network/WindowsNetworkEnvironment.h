/*
 * Copyright (c) 2011 Tobias Markmann
 * Licensed under the simplified BSD license.
 * See Documentation/Licenses/BSD-simplified.txt for more information.
 */

#pragma once

#include <vector>
#include <Swiften/Base/boost_bsignals.h>

#include <Swiften/Network/NetworkEnvironment.h>

namespace Swift {
	class WindowsNetworkEnvironment : public NetworkEnvironment {
		public:
			std::vector<NetworkInterface> getNetworkInterfaces() const;
	};
}
