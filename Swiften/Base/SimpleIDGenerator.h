/*
 * Copyright (c) 2010-2016 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <string>

#include <Swiften/Base/API.h>
#include <Swiften/Base/IDGenerator.h>
#include <Swiften/Base/Override.h>

namespace Swift {

    /**
     * @brief The SimpleIDGenerator class implements a IDGenerator generating consecutive ID strings from
     * the lower case latin alphabet.
     */

    class SWIFTEN_API SimpleIDGenerator : public IDGenerator {
        public:
            SimpleIDGenerator();
            ~SimpleIDGenerator();

            std::string generateID() SWIFTEN_OVERRIDE;

        private:
            std::string currentID;
    };
}
