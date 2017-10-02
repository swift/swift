/*
 * Copyright (c) 2010-2016 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <deque>
#include <memory>

#include <boost/signals2.hpp>

#include <Swiften/Base/API.h>
#include <Swiften/Elements/Stanza.h>

namespace Swift {
    class SWIFTEN_API StanzaAckRequester {
        public:
            StanzaAckRequester();

            void handleStanzaSent(std::shared_ptr<Stanza> stanza);
            void handleAckReceived(unsigned int handledStanzasCount);

        public:
            boost::signals2::signal<void ()> onRequestAck;
            boost::signals2::signal<void (std::shared_ptr<Stanza>)> onStanzaAcked;

        private:
            friend class StanzaAckRequesterTest;
            unsigned int lastHandledStanzasCount;
            std::deque<std::shared_ptr<Stanza> > unackedStanzas;
    };

}
