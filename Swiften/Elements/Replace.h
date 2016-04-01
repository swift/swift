/*
 * Copyright (c) 2011 Vlad Voicu
 * Licensed under the Simplified BSD license.
 * See Documentation/Licenses/BSD-simplified.txt for more information.
 */

/*
 * Copyright (c) 2015-2016 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <memory>
#include <string>

#include <Swiften/Base/API.h>
#include <Swiften/Elements/Payload.h>

namespace Swift {
    class SWIFTEN_API Replace : public Payload {
        public:
            typedef std::shared_ptr<Replace> ref;
            Replace(const std::string& id = std::string()) : replaceID_(id) {}
            const std::string& getID() const {
                return replaceID_;
            }
            void setID(const std::string& id) {
                replaceID_ = id;
            }
        private:
            std::string replaceID_;
    };
}
