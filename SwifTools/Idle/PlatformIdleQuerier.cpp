/*
 * Copyright (c) 2010 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#include <SwifTools/Idle/PlatformIdleQuerier.h>
#include <Swiften/Base/Platform.h>

#if defined(SWIFTEN_PLATFORM_MACOSX) && defined(HAVE_IOKIT) && !defined(SWIFTEN_PLATFORM_IPHONE)
#include <SwifTools/Idle/MacOSXIdleQuerier.h>
#elif defined(SWIFTEN_PLATFORM_WINDOWS)
#include <SwifTools/Idle/WindowsIdleQuerier.h>
#elif defined(HAVE_XSS)
#include <SwifTools/Idle/XSSIdleQuerier.h>
#endif
#include <SwifTools/Idle/DummyIdleQuerier.h>

#include <cassert>
#include <iostream>

namespace Swift {

PlatformIdleQuerier::PlatformIdleQuerier() : querier(NULL) {
#if defined(SWIFTEN_PLATFORM_MACOSX)
#if defined(HAVE_IOKIT) && !defined(SWIFTEN_PLATFORM_IPHONE)
	querier = new MacOSXIdleQuerier();
#else
	querier = new DummyIdleQuerier();
#endif
#elif defined(SWIFTEN_PLATFORM_WINDOWS)
	querier = new WindowsIdleQuerier();
#elif defined(HAVE_XSS)
	querier = new XSSIdleQuerier();
#else
	querier = new DummyIdleQuerier();
#endif
}

PlatformIdleQuerier::~PlatformIdleQuerier() {
	delete querier;
}

}
