#include "SwifTools/Idle/PlatformIdleQuerier.h"
#include "Swiften/Base/Platform.h"

#if defined(SWIFTEN_PLATFORM_MACOSX)
#include "SwifTools/Idle/MacOSXIdleQuerier.h"
#elif defined(HAVE_XSS)
#include "SwifTools/Idle/XSSIdleQuerier.h"
#else
#include "SwifTools/Idle/DummyIdleQuerier.h"
#endif

#include <cassert>
#include <iostream>

namespace Swift {

PlatformIdleQuerier::PlatformIdleQuerier() : querier(NULL) {
#if defined(SWIFTEN_PLATFORM_MACOSX)
	querier = new MacOSXIdleQuerier();
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
