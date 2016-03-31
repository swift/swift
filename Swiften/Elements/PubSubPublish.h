/*
 * Copyright (c) 2013-2016 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <string>
#include <vector>

#include <boost/shared_ptr.hpp>

#include <Swiften/Base/API.h>
#include <Swiften/Base/Override.h>
#include <Swiften/Elements/Payload.h>
#include <Swiften/Elements/PubSubItem.h>
#include <Swiften/Elements/PubSubPayload.h>

namespace Swift {
    class SWIFTEN_API PubSubPublish : public PubSubPayload {
        public:

            PubSubPublish();

            virtual ~PubSubPublish();

            const std::string& getNode() const {
                return node;
            }

            void setNode(const std::string& value) {
                this->node = value ;
            }

            const std::vector< boost::shared_ptr<PubSubItem> >& getItems() const {
                return items;
            }

            void setItems(const std::vector< boost::shared_ptr<PubSubItem> >& value) {
                this->items = value ;
            }

            void addItem(boost::shared_ptr<PubSubItem> value) {
                this->items.push_back(value);
            }


        private:
            std::string node;
            std::vector< boost::shared_ptr<PubSubItem> > items;
    };
}
