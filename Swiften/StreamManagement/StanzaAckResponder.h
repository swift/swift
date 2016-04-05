/*
 * Copyright (c) 2010-2016 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <memory>

#include <boost/signals2.hpp>

#include <Swiften/Base/API.h>
#include <Swiften/Elements/Stanza.h>

namespace Swift {
    class SWIFTEN_API StanzaAckResponder {
        public:
            StanzaAckResponder();

            void handleStanzaReceived();
            void handleAckRequestReceived();

        public:
            boost::signals2::signal<void (unsigned int /* handledStanzaCount */)> onAck;

        private:
            friend class StanzaAckResponderTest;
            unsigned int handledStanzasCount;
    };

}
