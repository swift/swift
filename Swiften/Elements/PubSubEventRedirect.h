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
    class SWIFTEN_API PubSubEventRedirect : public Payload {
        public:

            PubSubEventRedirect();

            virtual ~PubSubEventRedirect();

            const std::string& getURI() const {
                return uri;
            }

            void setURI(const std::string& value) {
                this->uri = value ;
            }


        private:
            std::string uri;
    };
}
