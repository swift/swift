/*
 * Copyright (c) 2010 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#pragma once

#include <iostream>

namespace Swift {
	extern bool logging;
}

#define SWIFT_LOG(severity) \
	if (!Swift::logging) {} else std::cerr << "[" << #severity << "] " << __FILE__ << ":" << __LINE__ << " " << __FUNCTION__ << ": "
