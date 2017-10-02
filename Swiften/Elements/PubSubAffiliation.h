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
    class SWIFTEN_API PubSubAffiliation : public Payload {
        public:
            enum Type {
                None,
                Member,
                Outcast,
                Owner,
                Publisher,
                PublishOnly
            };

            PubSubAffiliation();
            PubSubAffiliation(const std::string& node) : node(node), type(None) {}
            virtual ~PubSubAffiliation();

            const std::string& getNode() const {
                return node;
            }

            void setNode(const std::string& value) {
                this->node = value ;
            }

            Type getType() const {
                return type;
            }

            void setType(Type value) {
                this->type = value ;
            }


        private:
            std::string node;
            Type type;
    };
}
