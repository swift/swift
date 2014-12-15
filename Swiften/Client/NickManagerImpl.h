/*
 * Copyright (c) 2010 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <Swiften/Client/NickManager.h>
#include <Swiften/Elements/VCard.h>
#include <Swiften/JID/JID.h>
#include <string>

namespace Swift {
	class VCardManager;

	class NickManagerImpl : public NickManager {
		public:
			NickManagerImpl(const JID& ownJID, VCardManager* vcardManager);
			~NickManagerImpl();

			std::string getOwnNick() const;
			void setOwnNick(const std::string& nick);

		private:
			void handleVCardReceived(const JID& jid, VCard::ref vCard);
			void updateOwnNickFromVCard(VCard::ref vcard);

		private:
			JID ownJID;
			VCardManager* vcardManager;
			std::string ownNick;
	};
}
