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
#include <Swiften/Elements/PubSubOwnerPayload.h>
#include <Swiften/Elements/PubSubOwnerSubscription.h>

namespace Swift {
    class SWIFTEN_API PubSubOwnerSubscriptions : public PubSubOwnerPayload {
        public:

            PubSubOwnerSubscriptions();

            virtual ~PubSubOwnerSubscriptions();

            const std::string& getNode() const {
                return node;
            }

            void setNode(const std::string& value) {
                this->node = value ;
            }

            const std::vector< std::shared_ptr<PubSubOwnerSubscription> >& getSubscriptions() const {
                return subscriptions;
            }

            void setSubscriptions(const std::vector< std::shared_ptr<PubSubOwnerSubscription> >& value) {
                this->subscriptions = value ;
            }

            void addSubscription(std::shared_ptr<PubSubOwnerSubscription> value) {
                this->subscriptions.push_back(value);
            }


        private:
            std::string node;
            std::vector< std::shared_ptr<PubSubOwnerSubscription> > subscriptions;
    };
}
