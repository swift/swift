/*
 * Copyright (c) 2010 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#pragma once

#include "Swiften/Client/Storages.h"

namespace Swift {
	class VCardMemoryStorage;

	class MemoryStorages : public Storages {
		public:
			MemoryStorages();
			~MemoryStorages();

			virtual VCardStorage* getVCardStorage() const;
			virtual AvatarStorage* getAvatarStorage() const;
			virtual CapsStorage* getCapsStorage() const;

		private:
			VCardMemoryStorage* vcardStorage;
			AvatarStorage* avatarStorage;
			CapsStorage* capsStorage;
	};
}
