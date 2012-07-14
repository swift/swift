/*
 * Copyright (c) 2010-2012 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#pragma once

#include <iostream>

#include <Swiften/Base/API.h>

namespace Swift {
	extern SWIFTEN_API bool logging;
	namespace LogDetail {
		// Only here to be able to statically check the correctness of the severity levers
		namespace Severity {
			enum {
				debug, info, warning, error
			};
		}
	}
}

#define SWIFT_LOG(severity) \
	if (!Swift::logging) {} else (void) LogDetail::Severity::severity, std::cerr << "[" << #severity << "] " << __FILE__ << ":" << __LINE__ << " " << __FUNCTION__ << ": "
