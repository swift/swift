/*
 * Copyright (c) 2010 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#pragma once

#include <map>
#include <string>
#include <boost/filesystem/path.hpp>

#include <Swiften/JID/JID.h>
#include "Swiften/Base/ByteArray.h"
#include "Swiften/Avatars/AvatarStorage.h"

namespace Swift {
	class AvatarFileStorage : public AvatarStorage {
		public:
			AvatarFileStorage(const boost::filesystem::path& avatarsDir, const boost::filesystem::path& avatarsFile);

			virtual bool hasAvatar(const std::string& hash) const;
			virtual void addAvatar(const std::string& hash, const ByteArray& avatar);
			virtual ByteArray getAvatar(const std::string& hash) const;

			virtual boost::filesystem::path getAvatarPath(const std::string& hash) const;

			virtual void setAvatarForJID(const JID& jid, const std::string& hash);
			virtual std::string getAvatarForJID(const JID& jid) const;

		private:
			void saveJIDAvatars();

		private:
			boost::filesystem::path avatarsDir;
			boost::filesystem::path avatarsFile;
			typedef std::map<JID, std::string> JIDAvatarMap;
			JIDAvatarMap jidAvatars;
	};

}
