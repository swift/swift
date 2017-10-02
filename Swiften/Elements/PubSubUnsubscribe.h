/*
 * Copyright (c) 2013-2017 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <string>

#include <boost/optional.hpp>

#include <Swiften/Base/API.h>
#include <Swiften/Elements/Payload.h>
#include <Swiften/Elements/PubSubPayload.h>
#include <Swiften/JID/JID.h>

namespace Swift {
    class SWIFTEN_API PubSubUnsubscribe : public PubSubPayload {
        public:

            PubSubUnsubscribe();

            virtual ~PubSubUnsubscribe();

            const boost::optional< std::string >& getNode() const {
                return node;
            }

            void setNode(const boost::optional< std::string >& value) {
                this->node = value ;
            }

            const JID& getJID() const {
                return jid;
            }

            void setJID(const JID& value) {
                this->jid = value ;
            }

            const boost::optional< std::string >& getSubscriptionID() const {
                return subscriptionID;
            }

            void setSubscriptionID(const boost::optional< std::string >& value) {
                this->subscriptionID = value ;
            }


        private:
            boost::optional< std::string > node;
            JID jid;
            boost::optional< std::string > subscriptionID;
    };
}
