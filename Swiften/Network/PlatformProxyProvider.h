/*
 * Copyright (c) 2010-2011 Thilo Cestonaro
 * Licensed under the simplified BSD license.
 * See Documentation/Licenses/BSD-simplified.txt for more information.
 */

#pragma once

#include <Swiften/Base/Platform.h>

#if defined(SWIFTEN_PLATFORM_MACOSX)
#include <Swiften/Network/MacOSXProxyProvider.h>
namespace Swift {
	typedef MacOSXProxyProvider PlatformProxyProvider;
}
#elif defined(SWIFTEN_PLATFORM_WIN32)
#include <Swiften/Network/WindowsProxyProvider.h>
namespace Swift {
	typedef WindowsProxyProvider PlatformProxyProvider;
}
#else
#include <Swiften/Network/UnixProxyProvider.h>
namespace Swift {
	typedef UnixProxyProvider PlatformProxyProvider;
}
#endif
