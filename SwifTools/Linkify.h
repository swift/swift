/*
 * Copyright (c) 2010-2016 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <string>
#include <vector>

namespace Swift {
    namespace Linkify {
        std::string linkify(const std::string&);
        /**
         * Parse the string for a URI. The string will be split by the URI, and the segments plus index of the URI returned.
         * If no URI is found the index will be result.size() (i.e. an invalid index)
         *
         * Examples:
         * "not a URI" -> <<"not a URI">, -1>
         * "http://swift.im" -> <<"http://swift.im">, 0
         * " See http://swift.im" -> <<" See ", "http://swift.im">, 1>
         * "Right, http://swift.im it is" -> <<"Right, ", "http://swift.im", " it is">, 1>
          */
        std::pair<std::vector<std::string>, size_t> splitLink(const std::string& text);
    }
}
