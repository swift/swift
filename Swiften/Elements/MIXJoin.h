/*
 * Copyright (c) 2017 Tarun Gupta
 * Licensed under the simplified BSD license.
 * See Documentation/Licenses/BSD-simplified.txt for more information.
 */

#pragma once

#include <memory>
#include <string>
#include <unordered_set>

#include <boost/optional.hpp>

#include <Swiften/Base/API.h>
#include <Swiften/Elements/Payload.h>
#include <Swiften/Elements/Form.h>
#include <Swiften/JID/JID.h>

namespace Swift {
    class SWIFTEN_API MIXJoin : public Payload {

        public:
            using ref = std::shared_ptr<MIXJoin>;

        public:

            MIXJoin() {}

            const boost::optional<JID>& getChannel() const {
                return channel_;
            }

            void setChannel(JID channel) {
                channel_ = channel;
            }

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

            void setForm(std::shared_ptr<Form> form) {
                form_ = form;
            }

            const std::shared_ptr<Form>& getForm() const {
                return form_;
            }

        private:
            boost::optional<JID> jid_;
            boost::optional<JID> channel_;
            std::unordered_set<std::string> subscribeItems_;
            std::shared_ptr<Form> form_;
            // FIXME: MIXInvitation to be implemented. boost::optional<MIXInvitation> invitation_;
    };
}
