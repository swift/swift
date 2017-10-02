/*
 * Copyright (c) 2010-2016 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <boost/filesystem/path.hpp>
#include <boost/signals2.hpp>

#include <Swiften/Base/API.h>
#include <Swiften/Base/ByteArray.h>

namespace Swift {
    class JID;

    class SWIFTEN_API AvatarManager {
        public:
            virtual ~AvatarManager();

            virtual ByteArray getAvatar(const JID&) const = 0;
            virtual boost::filesystem::path getAvatarPath(const JID&) const = 0;

            boost::signals2::signal<void (const JID&)> onAvatarChanged;
    };
}
