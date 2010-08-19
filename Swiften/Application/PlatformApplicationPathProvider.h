/*
 * Copyright (c) 2010 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#pragma once

#include "Swiften/Base/Platform.h"

#if defined(SWIFTEN_PLATFORM_MACOSX)
#include "Swiften/Application/MacOSXApplicationPathProvider.h"
namespace Swift {
	typedef MacOSXApplicationPathProvider PlatformApplicationPathProvider;
}
#elif defined(SWIFTEN_PLATFORM_WIN32)
#include "Swiften/Application/WindowsApplicationPathProvider.h"
namespace Swift {
	typedef WindowsApplicationPathProvider PlatformApplicationPathProvider;
}
#else
#include "Swiften/Application/UnixApplicationPathProvider.h"
namespace Swift {
	typedef UnixApplicationPathProvider PlatformApplicationPathProvider;
}
#endif
