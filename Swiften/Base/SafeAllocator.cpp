/*
 * Copyright (c) 2013 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#include <Swiften/Base/SafeByteArray.h>

#include <Swiften/Base/Platform.h>
#ifdef SWIFTEN_PLATFORM_WINDOWS
#include <windows.h>
#endif

namespace Swift {

void secureZeroMemory(char* memory, size_t numberOfBytes) {
#ifdef SWIFTEN_PLATFORM_WINDOWS
	SecureZeroMemory(memory, numberOfBytes);
#else
	volatile char* p = memory;
	for (size_t i = 0; i < numberOfBytes; ++i) {
		*(p++) = 0;
	}
#endif
}

}
