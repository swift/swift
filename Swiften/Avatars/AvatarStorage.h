/*
 * Copyright (c) 2010-2016 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <string>

#include <boost/filesystem/path.hpp>

#include <Swiften/Base/API.h>
#include <Swiften/Base/ByteArray.h>

namespace Swift {
    class JID;

    class SWIFTEN_API AvatarStorage {
        public:
            virtual ~AvatarStorage();

            virtual bool hasAvatar(const std::string& hash) const = 0;
            virtual void addAvatar(const std::string& hash, const ByteArray& avatar) = 0;
            virtual ByteArray getAvatar(const std::string& hash) const = 0;
            virtual boost::filesystem::path getAvatarPath(const std::string& hash) const = 0;

            virtual void setAvatarForJID(const JID& jid, const std::string& hash) = 0;
            virtual std::string getAvatarForJID(const JID& jid) const = 0;
    };

}
