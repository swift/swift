/*
 * Copyright (c) 2010-2017 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <string>

#include <Swiften/Base/API.h>
#include <Swiften/Base/IDGenerator.h>

namespace Swift {

    /**
     * @brief The SimpleIDGenerator class implements a IDGenerator generating consecutive ID strings from
     * the lower case latin alphabet.
     */

    class SWIFTEN_API SimpleIDGenerator : public IDGenerator {
        public:
            SimpleIDGenerator();
            ~SimpleIDGenerator() override;

            std::string generateID() override;

        private:
            std::string currentID;
    };
}
