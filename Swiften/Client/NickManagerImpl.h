/*
 * Copyright (c) 2010 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#pragma once

#include <Swiften/Client/NickManager.h>
#include <Swiften/Elements/VCard.h>
#include <Swiften/JID/JID.h>
#include <Swiften/Base/String.h>

namespace Swift {
	class VCardManager;

	class NickManagerImpl : public NickManager {
		public:
			NickManagerImpl(const JID& ownJID, VCardManager* vcardManager);
			~NickManagerImpl();

			String getOwnNick() const;
			void setOwnNick(const String& nick);

		private:
			void handleVCardReceived(const JID& jid, VCard::ref vCard);

		private:
			JID ownJID;
			VCardManager* vcardManager;
			String ownNick;
	};
}
