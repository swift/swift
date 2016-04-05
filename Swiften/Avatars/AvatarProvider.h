/*
 * Copyright (c) 2010-2016 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <string>

#include <boost/signals2.hpp>

#include <Swiften/Base/API.h>

namespace Swift {
    class JID;

    class SWIFTEN_API AvatarProvider {
        public:
            virtual ~AvatarProvider();

            virtual boost::optional<std::string> getAvatarHash(const JID&) const = 0;

            boost::signals2::signal<void (const JID&)> onAvatarChanged;
    };
}
