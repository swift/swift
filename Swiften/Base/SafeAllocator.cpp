/*
 * Copyright (c) 2013 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#include <Swiften/Base/SafeByteArray.h>

#include <Swiften/Base/Platform.h>
#ifdef SWIFTEN_PLATFORM_WINDOWS
#include <windows.h>
#endif

namespace Swift {

SWIFTEN_API void secureZeroMemory(char* memory, size_t numberOfBytes) {
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
