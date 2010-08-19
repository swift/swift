/*
 * Copyright (c) 2010 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#pragma once

#include "Swiften/Base/Platform.h"


#if defined(SWIFTEN_PLATFORM_MACOSX)
#include "Swiften/Application/MacOSX/MacOSXApplication.h"
namespace Swift {
	typedef MacOSXApplication PlatformApplication;
}
#elif defined(SWIFTEN_PLATFORM_WIN32)
#include "Swiften/Application/WindowsApplication.h"
namespace Swift {
	typedef WindowsApplication PlatformApplication;
}
#else
#include "Swiften/Application/UnixApplication.h"
namespace Swift {
	typedef UnixApplication PlatformApplication;
}
#endif
