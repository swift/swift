/*
 * Copyright (c) 2013-2016 Isode Limited.
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
    class SWIFTEN_API PubSubRetract : public PubSubPayload {
        public:

            PubSubRetract();

            virtual ~PubSubRetract();

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

            boost::optional<bool> isNotify() const {
                return notify;
            }

            void setNotify(const boost::optional<bool>& value) {
                this->notify = value ;
            }


        private:
            std::string node;
            std::vector< std::shared_ptr<PubSubItem> > items;
            boost::optional<bool> notify;
    };
}
