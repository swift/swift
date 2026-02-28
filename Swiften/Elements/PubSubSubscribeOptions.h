/*
 * Copyright (c) 2013-2017 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <Swiften/Base/API.h>
#include <Swiften/Elements/Payload.h>

namespace Swift {
    class SWIFTEN_API PubSubSubscribeOptions : public Payload {
        public:

            PubSubSubscribeOptions();

            virtual ~PubSubSubscribeOptions();

            bool isRequired() const {
                return required;
            }

            void setRequired(bool value) {
                this->required = value ;
            }


        private:
            bool required;
    };
}
