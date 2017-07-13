/*
 * Copyright (c) 2013-2017 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <memory>
#include <string>
#include <vector>

#include <Swiften/Base/API.h>
#include <Swiften/Elements/Payload.h>
#include <Swiften/Elements/PubSubEventItem.h>
#include <Swiften/Elements/PubSubEventPayload.h>
#include <Swiften/Elements/PubSubEventRetract.h>

namespace Swift {
    class SWIFTEN_API PubSubEventItems : public PubSubEventPayload {
        public:

            PubSubEventItems();

            virtual ~PubSubEventItems();

            const std::string& getNode() const {
                return node;
            }

            void setNode(const std::string& value) {
                this->node = value ;
            }

            const std::vector< std::shared_ptr<PubSubEventItem> >& getItems() const {
                return items;
            }

            void setItems(const std::vector< std::shared_ptr<PubSubEventItem> >& value) {
                this->items = value ;
            }

            void addItem(std::shared_ptr<PubSubEventItem> value) {
                this->items.push_back(value);
            }

            const std::vector< std::shared_ptr<PubSubEventRetract> >& getRetracts() const {
                return retracts;
            }

            void setRetracts(const std::vector< std::shared_ptr<PubSubEventRetract> >& value) {
                this->retracts = value ;
            }

            void addRetract(std::shared_ptr<PubSubEventRetract> value) {
                this->retracts.push_back(value);
            }


        private:
            std::string node;
            std::vector< std::shared_ptr<PubSubEventItem> > items;
            std::vector< std::shared_ptr<PubSubEventRetract> > retracts;
    };
}
