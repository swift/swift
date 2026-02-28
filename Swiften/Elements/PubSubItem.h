/*
 * Copyright (c) 2013-2018 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <memory>
#include <string>
#include <vector>

#include <Swiften/Base/API.h>
#include <Swiften/Elements/Payload.h>

namespace Swift {
    class SWIFTEN_API PubSubItem : public Payload {
        public:

            PubSubItem();
            PubSubItem(const std::string& id);

            virtual ~PubSubItem();

            const std::vector< std::shared_ptr<Payload> >& getData() const {
                return data_;
            }

            void setData(const std::vector< std::shared_ptr<Payload> >& value) {
                this->data_ = value ;
            }

            void addData(std::shared_ptr<Payload> value) {
                this->data_.push_back(value);
            }

            const std::string& getID() const {
                return id_;
            }

            void setID(const std::string& value) {
                this->id_ = value ;
            }

        private:
            std::vector< std::shared_ptr<Payload> > data_;
            std::string id_;
    };
}
