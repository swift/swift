/*
 * Copyright (c) 2015-2016 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <memory>

#include <Swiften/JID/JID.h>

#include <Swift/Controllers/XMPPEvents/StanzaEvent.h>

namespace Swift {
    class IncomingFileTransferEvent : public StanzaEvent {
        public:
            typedef std::shared_ptr<IncomingFileTransferEvent> ref;

            IncomingFileTransferEvent(const JID& sender) : sender_(sender) {}

            const JID& getSender() const {
                return sender_;
            }

        private:
            JID sender_;
    };
}

