/*
 * Copyright (c) 2010 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <string>

#include <Swiften/Base/API.h>
#include <Swiften/Base/boost_bsignals.h>

namespace Swift {
    class JID;

    class SWIFTEN_API AvatarProvider {
        public:
            virtual ~AvatarProvider();

            virtual boost::optional<std::string> getAvatarHash(const JID&) const = 0;

            boost::signal<void (const JID&)> onAvatarChanged;
    };
}
