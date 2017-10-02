/*
 * Copyright (c) 2010-2016 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <memory>

#include <Swiften/Elements/Stanza.h>

namespace Swift {
    class ServerSession {
        public:
            virtual ~ServerSession();

            virtual const JID& getJID() const = 0;
            virtual int getPriority() const = 0;

            virtual void sendStanza(std::shared_ptr<Stanza>) = 0;
    };
}
