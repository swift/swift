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
    class SWIFTEN_API PubSubEventAssociate : public Payload {
        public:

            PubSubEventAssociate();

            virtual ~PubSubEventAssociate();

            const std::string& getNode() const {
                return node;
            }

            void setNode(const std::string& value) {
                this->node = value ;
            }


        private:
            std::string node;
    };
}
