/*
 * Copyright (c) 2010-2015 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <Swiften/Base/API.h>
#include <Swiften/Elements/Presence.h>

namespace Swift {
    class SWIFTEN_API PresenceSender {
        public:
            virtual ~PresenceSender();

            virtual void sendPresence(Presence::ref) = 0;

            virtual bool isAvailable() const = 0;
    };
}
