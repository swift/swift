/*
 * Copyright (c) 2010-2016 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <cassert>

#include <boost/shared_ptr.hpp>

#include <Swiften/Elements/Message.h>

#include <Swift/Controllers/XMPPEvents/StanzaEvent.h>

namespace Swift {
    class MessageEvent : public StanzaEvent {
        public:
            typedef boost::shared_ptr<MessageEvent> ref;

            MessageEvent(boost::shared_ptr<Message> stanza) : stanza_(stanza), targetsMe_(true) {}

            boost::shared_ptr<Message> getStanza() {return stanza_;}

            bool isReadable() {
                return getStanza()->isError() || !getStanza()->getBody().get_value_or("").empty();
            }

            void read() {
                assert (isReadable());
                conclude();
            }

            void setTargetsMe(bool targetsMe) {
                targetsMe_ = targetsMe;
            }

            bool targetsMe() const {
                return targetsMe_;
            }

        private:
            boost::shared_ptr<Message> stanza_;
            bool targetsMe_;
    };
}

