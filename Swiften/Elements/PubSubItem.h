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

namespace Swift {
    class SWIFTEN_API PubSubItem : public Payload {
        public:

            PubSubItem();

            virtual ~PubSubItem();

            const std::vector< boost::shared_ptr<Payload> >& getData() const {
                return data;
            }

            void setData(const std::vector< boost::shared_ptr<Payload> >& value) {
                this->data = value ;
            }

            void addData(boost::shared_ptr<Payload> value) {
                this->data.push_back(value);
            }

            const std::string& getID() const {
                return id;
            }

            void setID(const std::string& value) {
                this->id = value ;
            }


        private:
            std::vector< boost::shared_ptr<Payload> > data;
            std::string id;
    };
}
