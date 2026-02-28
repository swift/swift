/*
 * Copyright (c) 2013 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <Swiften/Base/API.h>

namespace Swift {
    class CryptoProvider;

    namespace PlatformCryptoProvider {
        SWIFTEN_API CryptoProvider* create();
    }
}
