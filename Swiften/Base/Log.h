/*
 * Copyright (c) 2010 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#pragma once

#include "Swiften/Base/Platform.h"
namespace Swift {
	extern bool logging;
}

#ifndef SWIFTEN_PLATFORM_WINDOWS
#define SWIFT_LOG(severity) \
	if (!Swift::logging) {} else std::cerr << __FILE__ << ":" << __LINE__ << " " << __FUNCTION__ << ": "
#else
#define SWIFT_LOG(severity) \
	if (!Swift::logging) {} else std::cerr
#endif
