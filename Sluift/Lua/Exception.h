/*
 * Copyright (c) 2013-2016 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <stdexcept>

#include <Swiften/Base/API.h>

namespace Swift {
    namespace Lua {
        class Exception : public std::runtime_error {
            public:
                Exception(const std::string& what);
                SWIFTEN_DEFAULT_COPY_CONSTRUCTOR(Exception)
                virtual ~Exception() SWIFTEN_NOEXCEPT;
        };
    }
}

