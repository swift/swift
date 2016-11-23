/*
 * Copyright (c) 2010-2016 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <string>
#include <utility> /* std::pair */
#include <vector>

#include <Swiften/Elements/StatusShow.h>

namespace Swift {
    typedef std::pair<StatusShow::Type, std::string> TypeStringPair;

    class PreviousStatusStore {
        public:
            PreviousStatusStore();
            ~PreviousStatusStore();
            void addStatus(StatusShow::Type status, const std::string& message);
            std::vector<TypeStringPair> getSuggestions(const std::string& message);

        private:
            std::vector<TypeStringPair> exactMatchSuggestions(StatusShow::Type status, const std::string& message);
            std::vector<TypeStringPair> store_;
    };
}
