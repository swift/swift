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
#include <Swiften/Elements/PubSubPayload.h>
#include <Swiften/Elements/PubSubSubscription.h>

namespace Swift {
    class SWIFTEN_API PubSubSubscriptions : public PubSubPayload {
        public:

            PubSubSubscriptions();
            PubSubSubscriptions(const std::string& node) : node(node) {}
            virtual ~PubSubSubscriptions();

            const boost::optional< std::string >& getNode() const {
                return node;
            }

            void setNode(const boost::optional< std::string >& value) {
                this->node = value ;
            }

            const std::vector< std::shared_ptr<PubSubSubscription> >& getSubscriptions() const {
                return subscriptions;
            }

            void setSubscriptions(const std::vector< std::shared_ptr<PubSubSubscription> >& value) {
                this->subscriptions = value ;
            }

            void addSubscription(std::shared_ptr<PubSubSubscription> value) {
                this->subscriptions.push_back(value);
            }


        private:
            boost::optional< std::string > node;
            std::vector< std::shared_ptr<PubSubSubscription> > subscriptions;
    };
}
