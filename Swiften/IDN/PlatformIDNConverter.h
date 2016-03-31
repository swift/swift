/*
 * Copyright (c) 2012 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <Swiften/Base/API.h>

namespace Swift {
    class IDNConverter;

    namespace PlatformIDNConverter {
        SWIFTEN_API IDNConverter* create();
    }
}
