/*
 * Copyright (c) 2010 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#pragma once

#include "Swiften/Avatars/AvatarManager.h"
#include "Swiften/Client/DummyStanzaChannel.h"

namespace Swift {
	class MockAvatarManager : public AvatarManager {
		public:
			MockAvatarManager();
			virtual ~MockAvatarManager();
			virtual String getAvatarHash(const JID&) const;
			virtual boost::filesystem::path getAvatarPath(const JID&) const;
			virtual void setAvatar(const JID&, const ByteArray& avatar);
		private:
			DummyStanzaChannel channel_;
	};
}
