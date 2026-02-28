/*
 * Copyright (c) 2013-2017 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <memory>
#include <string>

#include <boost/optional.hpp>

#include <Swiften/Base/API.h>
#include <Swiften/Elements/Form.h>
#include <Swiften/Elements/Payload.h>
#include <Swiften/Elements/PubSubOwnerPayload.h>

namespace Swift {
    class SWIFTEN_API PubSubOwnerConfigure : public PubSubOwnerPayload {
        public:

            PubSubOwnerConfigure();
            PubSubOwnerConfigure(const std::string& node) : node(node) {}
            virtual ~PubSubOwnerConfigure();

            const boost::optional< std::string >& getNode() const {
                return node;
            }

            void setNode(const boost::optional< std::string >& value) {
                this->node = value ;
            }

            std::shared_ptr<Form> getData() const {
                return data;
            }

            void setData(std::shared_ptr<Form> value) {
                this->data = value ;
            }


        private:
            boost::optional< std::string > node;
            std::shared_ptr<Form> data;
    };
}
