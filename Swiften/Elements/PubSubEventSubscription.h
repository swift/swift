/*
 * Copyright (c) 2013-2017 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <string>

#include <boost/date_time/posix_time/posix_time_types.hpp>
#include <boost/optional.hpp>

#include <Swiften/Base/API.h>
#include <Swiften/Elements/Payload.h>
#include <Swiften/Elements/PubSubEventPayload.h>
#include <Swiften/JID/JID.h>

namespace Swift {
    class SWIFTEN_API PubSubEventSubscription : public PubSubEventPayload {
        public:
            enum SubscriptionType {
                None,
                Pending,
                Subscribed,
                Unconfigured
            };

            PubSubEventSubscription();

            virtual ~PubSubEventSubscription();

            const std::string& getNode() const {
                return node;
            }

            void setNode(const std::string& value) {
                this->node = value ;
            }

            const JID& getJID() const {
                return jid;
            }

            void setJID(const JID& value) {
                this->jid = value ;
            }

            SubscriptionType getSubscription() const {
                return subscription;
            }

            void setSubscription(SubscriptionType value) {
                this->subscription = value ;
            }

            const boost::optional< std::string >& getSubscriptionID() const {
                return subscriptionID;
            }

            void setSubscriptionID(const boost::optional< std::string >& value) {
                this->subscriptionID = value ;
            }

            const boost::posix_time::ptime& getExpiry() const {
                return expiry;
            }

            void setExpiry(const boost::posix_time::ptime& value) {
                this->expiry = value ;
            }


        private:
            std::string node;
            JID jid;
            SubscriptionType subscription;
            boost::optional< std::string > subscriptionID;
            boost::posix_time::ptime expiry;
    };
}
