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
#include <Swiften/Elements/Payload.h>
#include <Swiften/Elements/PubSubEventAssociate.h>
#include <Swiften/Elements/PubSubEventDisassociate.h>
#include <Swiften/Elements/PubSubEventPayload.h>

namespace Swift {
    class SWIFTEN_API PubSubEventCollection : public PubSubEventPayload {
        public:

            PubSubEventCollection();

            virtual ~PubSubEventCollection();

            const boost::optional< std::string >& getNode() const {
                return node;
            }

            void setNode(const boost::optional< std::string >& value) {
                this->node = value ;
            }

            std::shared_ptr<PubSubEventDisassociate> getDisassociate() const {
                return disassociate;
            }

            void setDisassociate(std::shared_ptr<PubSubEventDisassociate> value) {
                this->disassociate = value ;
            }

            std::shared_ptr<PubSubEventAssociate> getAssociate() const {
                return associate;
            }

            void setAssociate(std::shared_ptr<PubSubEventAssociate> value) {
                this->associate = value ;
            }


        private:
            boost::optional< std::string > node;
            std::shared_ptr<PubSubEventDisassociate> disassociate;
            std::shared_ptr<PubSubEventAssociate> associate;
    };
}
