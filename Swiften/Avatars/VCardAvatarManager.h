/*
 * Copyright (c) 2010-2013 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#pragma once

#include <Swiften/Base/API.h>
#include <Swiften/Avatars/AvatarProvider.h>
#include <Swiften/JID/JID.h>

namespace Swift {
	class MUCRegistry;
	class AvatarStorage;
	class VCardManager;
	class CryptoProvider;

	class SWIFTEN_API VCardAvatarManager : public AvatarProvider {
		public:
			VCardAvatarManager(VCardManager*, AvatarStorage*, CryptoProvider* crypto, MUCRegistry* = NULL);

			boost::optional<std::string> getAvatarHash(const JID&) const;

		private:
			void handleVCardChanged(const JID& from);
			JID getAvatarJID(const JID& o) const;

		private:
			VCardManager* vcardManager_;
			AvatarStorage* avatarStorage_;
			CryptoProvider* crypto_;
			MUCRegistry* mucRegistry_;
	};
}
