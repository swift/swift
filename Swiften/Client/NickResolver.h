/*
 * Copyright (c) 2010 Kevin Smith
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#include <map>
#include <boost/shared_ptr.hpp>

#include <Swiften/Base/boost_bsignals.h>
#include <string>
#include <Swiften/JID/JID.h>
#include <Swiften/Elements/VCard.h>

namespace Swift {
	class XMPPRoster;
	class MUCRegistry;
	class VCardManager;
	class NickResolver {
		public:
			NickResolver(const JID& ownJID, XMPPRoster* xmppRoster, VCardManager* vcardManager, MUCRegistry* mucRegistry);

			std::string jidToNick(const JID& jid);

			boost::signal<void (const JID&, const std::string& /*previousNick*/)> onNickChanged;

		private:
			void handleVCardReceived(const JID& jid, VCard::ref vCard);
			void handleJIDUpdated(const JID& jid, const std::string& previousNick, const std::vector<std::string>& groups);
			void handleJIDAdded(const JID& jid);

		private:
			JID ownJID_;
			std::string ownNick_;
			XMPPRoster* xmppRoster_;
			MUCRegistry* mucRegistry_;
			VCardManager* vcardManager_;
	};
}

