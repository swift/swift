/*
 * Copyright (c) 2013 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <string>
#include <vector>

namespace Swift {
    class Completer {
        public:
            virtual ~Completer();

            virtual std::vector<std::string> getCompletions(const std::string& buffer, int start, int end) = 0;
    };
}
