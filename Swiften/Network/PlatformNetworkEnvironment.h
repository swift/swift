/*
 * Copyright (c) 2011 Tobias Markmann
 * Licensed under the simplified BSD license.
 * See Documentation/Licenses/BSD-simplified.txt for more information.
 */

#pragma once

#include <Swiften/Base/Platform.h>

#if defined(SWIFTEN_PLATFORM_MACOSX)
#include <Swiften/Network/UnixNetworkEnvironment.h>
namespace Swift {
	typedef UnixNetworkEnvironment PlatformNetworkEnvironment;
}
#elif defined(SWIFTEN_PLATFORM_WIN32)
#include <Swiften/Network/WindowsNetworkEnvironment.h>
namespace Swift {
	typedef WindowsNetworkEnvironment PlatformNetworkEnvironment;
}
#else
#include <Swiften/Network/UnixNetworkEnvironment.h>
namespace Swift {
	typedef UnixNetworkEnvironment PlatformNetworkEnvironment;
}
#endif
