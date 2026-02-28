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

namespace Swift {
    class SWIFTEN_API PubSubEventItem : public Payload {
        public:

            PubSubEventItem();

            virtual ~PubSubEventItem();

            const boost::optional< std::string >& getNode() const {
                return node;
            }

            void setNode(const boost::optional< std::string >& value) {
                this->node = value ;
            }

            const boost::optional< std::string >& getPublisher() const {
                return publisher;
            }

            void setPublisher(const boost::optional< std::string >& value) {
                this->publisher = value ;
            }

            const std::vector< std::shared_ptr<Payload> >& getData() const {
                return data;
            }

            void setData(const std::vector< std::shared_ptr<Payload> >& value) {
                this->data = value ;
            }

            void addData(std::shared_ptr<Payload> value) {
                this->data.push_back(value);
            }

            const boost::optional< std::string >& getID() const {
                return id;
            }

            void setID(const boost::optional< std::string >& value) {
                this->id = value ;
            }


        private:
            boost::optional< std::string > node;
            boost::optional< std::string > publisher;
            std::vector< std::shared_ptr<Payload> > data;
            boost::optional< std::string > id;
    };
}
