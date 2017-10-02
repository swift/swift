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
#include <Swiften/Elements/PubSubPayload.h>
#include <Swiften/Elements/PubSubSubscribeOptions.h>
#include <Swiften/JID/JID.h>

namespace Swift {
    class SWIFTEN_API PubSubSubscription : public PubSubPayload {
        public:
            enum SubscriptionType {
                None,
                Pending,
                Subscribed,
                Unconfigured
            };

            PubSubSubscription();

            virtual ~PubSubSubscription();

            const boost::optional< std::string >& getNode() const {
                return node;
            }

            void setNode(const boost::optional< std::string >& value) {
                this->node = value ;
            }

            const boost::optional< std::string >& getSubscriptionID() const {
                return subscriptionID;
            }

            void setSubscriptionID(const boost::optional< std::string >& value) {
                this->subscriptionID = value ;
            }

            const JID& getJID() const {
                return jid;
            }

            void setJID(const JID& value) {
                this->jid = value ;
            }

            std::shared_ptr<PubSubSubscribeOptions> getOptions() const {
                return options;
            }

            void setOptions(std::shared_ptr<PubSubSubscribeOptions> value) {
                this->options = value ;
            }

            SubscriptionType getSubscription() const {
                return subscription;
            }

            void setSubscription(SubscriptionType value) {
                this->subscription = value ;
            }


        private:
            boost::optional< std::string > node;
            boost::optional< std::string > subscriptionID;
            JID jid;
            std::shared_ptr<PubSubSubscribeOptions> options;
            SubscriptionType subscription;
    };
}
