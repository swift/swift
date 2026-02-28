/*
 * Copyright (c) 2012-2019 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <Swiften/Base/API.h>

#include <memory>

namespace Swift {
    class IDNConverter;

    namespace PlatformIDNConverter {
        SWIFTEN_API std::unique_ptr<IDNConverter> create();
    }
}
