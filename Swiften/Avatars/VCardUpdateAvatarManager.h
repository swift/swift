/*
 * Copyright (c) 2010 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#pragma once

#include <boost/shared_ptr.hpp>
#include <map>

#include <Swiften/Avatars/AvatarProvider.h>
#include <Swiften/JID/JID.h>
#include <Swiften/Elements/Presence.h>
#include <Swiften/Elements/VCard.h>
#include <Swiften/Elements/ErrorPayload.h>

namespace Swift {
	class MUCRegistry;
	class AvatarStorage;
	class StanzaChannel;
	class VCardManager;

	class VCardUpdateAvatarManager : public AvatarProvider, public boost::bsignals::trackable {
		public:
			VCardUpdateAvatarManager(VCardManager*, StanzaChannel*, AvatarStorage*, MUCRegistry* = NULL);

			std::string getAvatarHash(const JID&) const;

		private:
			void handlePresenceReceived(boost::shared_ptr<Presence>);
			void handleStanzaChannelAvailableChanged(bool);
			void handleVCardChanged(const JID& from, VCard::ref);
			void setAvatarHash(const JID& from, const std::string& hash);
			JID getAvatarJID(const JID& o) const;

		private:
			VCardManager* vcardManager_;
			StanzaChannel* stanzaChannel_;
			AvatarStorage* avatarStorage_;
			MUCRegistry* mucRegistry_;
			std::map<JID, std::string> avatarHashes_;
	};
}
