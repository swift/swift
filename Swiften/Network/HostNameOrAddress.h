/*
 * Copyright (c) 2012 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */
#pragma once

#include <string>
#include <Swiften/Network/HostAddress.h>
#include <boost/variant.hpp>

namespace Swift {
	typedef boost::variant<std::string, HostAddress> HostNameOrAddress;

	std::string toString(const HostNameOrAddress& address);
}
