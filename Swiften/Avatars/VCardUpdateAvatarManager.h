/*
 * Copyright (c) 2010 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#pragma once

#include <boost/filesystem.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/optional.hpp>
#include <map>

#include "Swiften/Base/boost_bsignals.h"
#include "Swiften/JID/JID.h"
#include "Swiften/Elements/Presence.h"
#include "Swiften/Elements/VCard.h"
#include "Swiften/Elements/ErrorPayload.h"

namespace Swift {
	class MUCRegistry;
	class AvatarStorage;
	class StanzaChannel;
	class VCardManager;

	class VCardUpdateAvatarManager {
		public:
			VCardUpdateAvatarManager(VCardManager*, StanzaChannel*, AvatarStorage*, MUCRegistry* = NULL);
			virtual ~VCardUpdateAvatarManager();

			virtual boost::filesystem::path getAvatarPath(const JID&) const;

		public:
			boost::signal<void (const JID&, const String& /*hash*/)> onAvatarChanged;

		private:
			void handlePresenceReceived(boost::shared_ptr<Presence>);
			void handleStanzaChannelAvailableChanged(bool);
			void handleVCardChanged(const JID& from, VCard::ref);
			void setAvatarHash(const JID& from, const String& hash);
			JID getAvatarJID(const JID& o) const;
			String getAvatarHash(const JID&) const;

		private:
			VCardManager* vcardManager_;
			StanzaChannel* stanzaChannel_;
			AvatarStorage* avatarStorage_;
			MUCRegistry* mucRegistry_;
			std::map<JID, String> avatarHashes_;
	};
}
