/*
 * Copyright (c) 2011 Tobias Markmann
 * Licensed under the simplified BSD license.
 * See Documentation/Licenses/BSD-simplified.txt for more information.
 */

/*
* Copyright (c) 2013-2014 Remko Tronçon and Kevin Smith
* Licensed under the GNU General Public License v3.
* See Documentation/Licenses/GPLv3.txt for more information.
*/

#pragma once

#include <vector>

#include <Swiften/Base/boost_bsignals.h>

#include <Swiften/Network/NetworkEnvironment.h>
#include <Swiften/Network/NetworkInterface.h>

namespace Swift {

	class SolarisNetworkEnvironment : public NetworkEnvironment {
		public:
			std::vector<NetworkInterface> getNetworkInterfaces() const;
	};

}
