/*
 * Copyright (c) 2013-2017 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <string>

#include <Swiften/Base/API.h>
#include <Swiften/Elements/Payload.h>

namespace Swift {
    class SWIFTEN_API PubSubEventRetract : public Payload {
        public:

            PubSubEventRetract();

            virtual ~PubSubEventRetract();

            const std::string& getID() const {
                return id;
            }

            void setID(const std::string& value) {
                this->id = value ;
            }


        private:
            std::string id;
    };
}
