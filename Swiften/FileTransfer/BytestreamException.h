/*
 * Copyright (c) 2010-2015 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <stdexcept>

#include <Swiften/Base/API.h>

namespace Swift {
    class SWIFTEN_API BytestreamException : public std::exception {
        public:
            BytestreamException() {
            }
    };
}
