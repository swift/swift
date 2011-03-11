/*
 * Copyright (c) 2010 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#pragma once

#include <map>
#include <string>

#include <Swiften/Base/ByteArray.h>
#include <Swiften/Avatars/AvatarStorage.h>
#include <Swiften/JID/JID.h>

namespace Swift {
	class AvatarMemoryStorage : public AvatarStorage {
		public:
			virtual bool hasAvatar(const std::string& hash) const { return avatars.find(hash) != avatars.end(); }
			virtual void addAvatar(const std::string& hash, const ByteArray& avatar) { avatars[hash] = avatar; }
			virtual ByteArray getAvatar(const std::string& hash) const {
				std::map<std::string, ByteArray>::const_iterator i = avatars.find(hash);
				return i == avatars.end() ? ByteArray() : i->second;
			}

			virtual boost::filesystem::path getAvatarPath(const std::string& hash) const {
				return boost::filesystem::path("/avatars") / hash;
			}

			virtual void setAvatarForJID(const JID& jid, const std::string& hash) {
				jidAvatars[jid] = hash;
			}

			virtual std::string getAvatarForJID(const JID& jid) const {
				std::map<JID, std::string>::const_iterator i = jidAvatars.find(jid);
				return i == jidAvatars.end() ? "" : i->second;
			}

		private:
			std::map<std::string, ByteArray> avatars;
			std::map<JID, std::string> jidAvatars;
	};
}
