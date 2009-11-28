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
