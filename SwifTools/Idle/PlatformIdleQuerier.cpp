#include "SwifTools/Idle/PlatformIdleQuerier.h"

#if defined(HAVE_XSS)
#include "SwifTools/Idle/XSSIdleQuerier.h"
#else
#include "SwifTools/Idle/DummyIdleQuerier.h"
#endif

#include <cassert>
#include <iostream>

namespace Swift {

PlatformIdleQuerier::PlatformIdleQuerier() : querier(NULL) {
#if defined(HAVE_XSS)
	querier = new XSSIdleQuerier();
#else
	querier = new DummyIdleQuerier();
#endif
}

PlatformIdleQuerier::~PlatformIdleQuerier() {
	delete querier;
}

}
