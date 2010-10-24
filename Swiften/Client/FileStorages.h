/*
 * Copyright (c) 2010 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#pragma once

#include <boost/filesystem.hpp>

#include "Swiften/Client/Storages.h"

namespace Swift {
	class VCardFileStorage;
	class AvatarFileStorage;
	class CapsFileStorage;
	class JID;

	class FileStorages : public Storages {
		public:
			FileStorages(const boost::filesystem::path& baseDir, const JID& jid);
			~FileStorages();

			virtual VCardStorage* getVCardStorage() const;
			virtual AvatarStorage* getAvatarStorage() const;
			virtual CapsStorage* getCapsStorage() const;

		private:
			VCardFileStorage* vcardStorage;
			AvatarFileStorage* avatarStorage;
			CapsFileStorage* capsStorage;
	};
}
