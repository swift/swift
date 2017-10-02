/*
 * Copyright (c) 2010 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <Swiften/Base/API.h>
#include <Swiften/Presence/PresenceSender.h>

namespace Swift {
    class StanzaChannel;

    class SWIFTEN_API StanzaChannelPresenceSender : public PresenceSender {
        public:
            StanzaChannelPresenceSender(StanzaChannel*);

            void sendPresence(Presence::ref);

            bool isAvailable() const;

        private:
            StanzaChannel* channel;
    };
}
