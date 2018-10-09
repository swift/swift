/*
 * Copyright (c) 2012-2018 Isode Limited.
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

IDNConverter* PlatformIDNConverter::create() {
#if defined(HAVE_LIBIDN)
    return new LibIDNConverter();
#elif defined(HAVE_ICU)
    return new ICUConverter();
#else
#if defined(NEED_IDN)
#error "No IDN implementation"
#else
    return nullptr;
#endif
#endif
}

}
