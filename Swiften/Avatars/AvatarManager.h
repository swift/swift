#pragma once

#include <boost/filesystem.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/optional.hpp>
#include <boost/signal.hpp>
#include <map>

#include "Swiften/JID/JID.h"
#include "Swiften/Elements/Presence.h"
#include "Swiften/Elements/VCard.h"
#include "Swiften/Elements/Error.h"

namespace Swift {
	class MUCRegistry;
	class AvatarStorage;
	class StanzaChannel;
	class IQRouter;

	class AvatarManager {
		public:
			AvatarManager(StanzaChannel*, IQRouter*, AvatarStorage*, MUCRegistry*);

			String getAvatarHash(const JID&) const;
			boost::filesystem::path getAvatarPath(const JID&) const;
			void setAvatar(const JID&, const ByteArray& avatar);

		public:
			boost::signal<void (const JID&, const String&)> onAvatarChanged;

		private:
			void handlePresenceReceived(boost::shared_ptr<Presence>);
			void handleVCardReceived(const JID& from, const String& hash, boost::shared_ptr<VCard>, const boost::optional<Error>&);
			void setAvatarHash(const JID& from, const String& hash);
			JID getAvatarJID(const JID& o) const;

		private:
			StanzaChannel* stanzaChannel_;
			IQRouter* iqRouter_;
			AvatarStorage* avatarStorage_;
			MUCRegistry* mucRegistry_;
			std::map<JID, String> avatarHashes_;
	};
}
