/*
 * Copyright (c) 2011-2015 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <string>

#include <Swiften/Base/API.h>
#include <Swiften/Elements/JingleContentPayload.h>

namespace Swift {
    class SWIFTEN_API JingleContentID {
        public:
            JingleContentID(const std::string& name, JingleContentPayload::Creator creator) : name(name), creator(creator) {
            }

            const std::string getName() const {
                return this->name;
            }

            JingleContentPayload::Creator getCreator() const {
                return this->creator;
            }

        private:
            std::string name;
            JingleContentPayload::Creator creator;
    };
}
