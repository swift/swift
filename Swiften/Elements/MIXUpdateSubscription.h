/*
 * Copyright (c) 2017 Tarun Gupta
 * Licensed under the simplified BSD license.
 * See Documentation/Licenses/BSD-simplified.txt for more information.
 */

#pragma once

#include <memory>
#include <unordered_set>

#include <boost/optional.hpp>

#include <Swiften/Base/API.h>
#include <Swiften/Elements/Payload.h>
#include <Swiften/JID/JID.h>

namespace Swift {
    class SWIFTEN_API MIXUpdateSubscription : public Payload {

        public:
            using ref = std::shared_ptr<MIXUpdateSubscription>;

        public:

            MIXUpdateSubscription() {}

            const boost::optional<JID>& getJID() const {
                return jid_;
            }

            void setJID(JID jid) {
                jid_ = jid;
            }

            const std::unordered_set<std::string>& getSubscriptions() const {
                return subscribeItems_;
            }

            void setSubscriptions(std::unordered_set<std::string> values) {
                subscribeItems_ = values ;
            }

            void addSubscription(std::string value) {
                subscribeItems_.insert(value);
            }

            bool hasSubscription(const std::string& value) const {
                return std::find(subscribeItems_.begin(), subscribeItems_.end(), value) != subscribeItems_.end();
            }

        private:
            boost::optional<JID> jid_;
            std::unordered_set<std::string> subscribeItems_;
    };
}
