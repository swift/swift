/*
 * Copyright (c) 2013-2017 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <memory>

#include <Swiften/Base/API.h>
#include <Swiften/Elements/Form.h>
#include <Swiften/Elements/Payload.h>
#include <Swiften/Elements/PubSubOwnerPayload.h>

namespace Swift {
    class SWIFTEN_API PubSubOwnerDefault : public PubSubOwnerPayload {
        public:

            PubSubOwnerDefault();

            virtual ~PubSubOwnerDefault();

            std::shared_ptr<Form> getData() const {
                return data;
            }

            void setData(std::shared_ptr<Form> value) {
                this->data = value ;
            }


        private:
            std::shared_ptr<Form> data;
    };
}
