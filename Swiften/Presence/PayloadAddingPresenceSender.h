/*
 * Copyright (c) 2010-2017 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <Swiften/Base/API.h>
#include <Swiften/Elements/Payload.h>
#include <Swiften/Presence/PresenceSender.h>

namespace Swift {
    /**
     * This presence sender adds payloads to outgoing presences.
     *
     * This class isn't meant to be used with directed presence.
     */
    class SWIFTEN_API PayloadAddingPresenceSender : public PresenceSender {
        public:
            PayloadAddingPresenceSender(PresenceSender*);

            void sendPresence(std::shared_ptr<Presence>);
            bool isAvailable() const;

            /**
             * Sets the payload to be added to outgoing presences.
             * If initial presence has been sent, this will resend the last sent presence
             * with an updated payload. Initial presence is reset when unavailable presence is
             * sent, or when reset() is called.
             */
            void setPayload(std::shared_ptr<Payload>);

             /**
              * Resets the presence sender.
              * This puts the presence sender back in the initial state (before initial
              * presence has been sent).
              * This also resets the chained sender.
              */
             void reset();

        private:
            std::shared_ptr<Presence> lastSentPresence;
            PresenceSender* sender;
            std::shared_ptr<Payload> payload;
    };
}
