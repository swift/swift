/*
 * Copyright (c) 2011-2016 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <memory>

#include <Swiften/Base/API.h>
#include <Swiften/Elements/RosterPayload.h>

namespace Swift {
    class SWIFTEN_API RosterStorage {
        public:
            virtual ~RosterStorage();

            virtual std::shared_ptr<RosterPayload> getRoster() const = 0;
            virtual void setRoster(std::shared_ptr<RosterPayload>) = 0;
    };
}
