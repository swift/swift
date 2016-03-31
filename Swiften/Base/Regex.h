/*
 * Copyright (c) 2013 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <string>

#include <Swiften/Base/API.h>

namespace Swift {

    namespace Regex {
        SWIFTEN_API std::string escape(const std::string& source);
    }

}
