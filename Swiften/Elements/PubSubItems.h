/*
 * Copyright (c) 2013-2017 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <memory>
#include <string>
#include <vector>

#include <boost/optional.hpp>

#include <Swiften/Base/API.h>
#include <Swiften/Elements/Payload.h>
#include <Swiften/Elements/PubSubItem.h>
#include <Swiften/Elements/PubSubPayload.h>

namespace Swift {
    class SWIFTEN_API PubSubItems : public PubSubPayload {
        public:

            PubSubItems();
            PubSubItems(const std::string& node) : node(node) {}
            virtual ~PubSubItems();

            const std::string& getNode() const {
                return node;
            }

            void setNode(const std::string& value) {
                this->node = value ;
            }

            const std::vector< std::shared_ptr<PubSubItem> >& getItems() const {
                return items;
            }

            void setItems(const std::vector< std::shared_ptr<PubSubItem> >& value) {
                this->items = value ;
            }

            void addItem(std::shared_ptr<PubSubItem> value) {
                this->items.push_back(value);
            }

            const boost::optional< unsigned int >& getMaximumItems() const {
                return maximumItems;
            }

            void setMaximumItems(const boost::optional< unsigned int >& value) {
                this->maximumItems = value ;
            }

            const boost::optional< std::string >& getSubscriptionID() const {
                return subscriptionID;
            }

            void setSubscriptionID(const boost::optional< std::string >& value) {
                this->subscriptionID = value ;
            }

        private:
            std::string node;
            std::vector< std::shared_ptr<PubSubItem> > items;
            boost::optional< unsigned int > maximumItems;
            boost::optional< std::string > subscriptionID;
    };
}
