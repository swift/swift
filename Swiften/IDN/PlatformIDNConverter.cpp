/*
 * Copyright (c) 2012 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
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
#error "No IDN implementation"
	return 0;
#endif
}

}
