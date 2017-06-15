/*
 * Copyright (c) 2017 Tarun Gupta
 * Licensed under the simplified BSD license.
 * See Documentation/Licenses/BSD-simplified.txt for more information.
 */

#pragma once

#include <memory>
#include <string>

#include <boost/optional.hpp>

#include <Swiften/Base/API.h>
#include <Swiften/Elements/Payload.h>
#include <Swiften/Elements/MIXSubscribe.h>
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

            void setChannel(const JID& channel) {
                channel_ = channel;
            }

            const boost::optional<JID>& getJID() const {
                return jid_;
            }

            void setJID(const JID& jid) {
                jid_ = jid;
            }

            const std::vector<MIXSubscribe::ref>& getSubscriptions() const {
                return subscribeItems_;
            }

            void setSubscriptions(const std::vector<MIXSubscribe::ref>& value) {
                subscribeItems_ = value ;
            }

            void addSubscription(MIXSubscribe::ref value) {
                subscribeItems_.push_back(value);
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
            std::vector<MIXSubscribe::ref> subscribeItems_;
            std::shared_ptr<Form> form_;
            // FIXME: MIXInvitation to be implemented. boost::optional<MIXInvitation> invitation_;
    };
}
