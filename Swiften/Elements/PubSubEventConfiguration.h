/*
 * Copyright (c) 2013-2016 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <string>

#include <boost/shared_ptr.hpp>

#include <Swiften/Base/API.h>
#include <Swiften/Base/Override.h>
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

            boost::shared_ptr<Form> getData() const {
                return data;
            }

            void setData(boost::shared_ptr<Form> value) {
                this->data = value ;
            }


        private:
            std::string node;
            boost::shared_ptr<Form> data;
    };
}
