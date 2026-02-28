/*
 * Copyright (c) 2011 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#include <Swiften/Base/SafeByteArray.h>

using namespace Swift;

namespace Swift {

SafeByteArray createSafeByteArray(const char* c) {
    SafeByteArray data;
    while (*c) {
        data.push_back(static_cast<unsigned char>(*c));
        ++c;
    }
    return data;
}

}
