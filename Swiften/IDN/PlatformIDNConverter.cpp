/*
 * Copyright (c) 2012-2019 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#include <Swiften/IDN/PlatformIDNConverter.h>
#if defined(HAVE_LIBIDN)
#include <Swiften/IDN/LibIDNConverter.h>
#elif defined(HAVE_ICU)
#include <Swiften/IDN/ICUConverter.h>
#endif

namespace Swift {

std::unique_ptr<IDNConverter> PlatformIDNConverter::create() {
#if defined(HAVE_LIBIDN)
    return std::make_unique<LibIDNConverter>();
#elif defined(HAVE_ICU)
    return std::make_unique<ICUConverter>();
#else
#if defined(NEED_IDN)
#error "No IDN implementation"
#else
    return {};
#endif
#endif
}

}
