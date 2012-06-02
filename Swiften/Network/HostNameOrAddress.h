/*
 * Copyright (c) 2012 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */
#pragma once

#include <string>
#include <Swiften/Network/HostAddress.h>
#include <boost/variant.hpp>

namespace Swift {
	typedef boost::variant<std::string, HostAddress> HostNameOrAddress;

	std::string toString(const HostNameOrAddress& address);
}
