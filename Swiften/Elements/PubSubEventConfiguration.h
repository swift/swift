/*
 * Copyright (c) 2013-2017 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <memory>
#include <string>

#include <Swiften/Base/API.h>
#include <Swiften/Elements/Form.h>
#include <Swiften/Elements/Payload.h>
#include <Swiften/Elements/PubSubEventPayload.h>

namespace Swift {
    class SWIFTEN_API PubSubEventConfiguration : public PubSubEventPayload {
        public:

            PubSubEventConfiguration();

            virtual ~PubSubEventConfiguration();

            const std::string& getNode() const {
                return node;
            }

            void setNode(const std::string& value) {
                this->node = value ;
            }

            std::shared_ptr<Form> getData() const {
                return data;
            }

            void setData(std::shared_ptr<Form> value) {
                this->data = value ;
            }


        private:
            std::string node;
            std::shared_ptr<Form> data;
    };
}
