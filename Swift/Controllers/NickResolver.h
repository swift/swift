/*
 * Copyright (c) 2010 Kevin Smith
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#ifndef SWIFTEN_NickResolver_H
#define SWIFTEN_NickResolver_H

#include <map>
#include <boost/signals.hpp>
#include <boost/shared_ptr.hpp>

#include "Swiften/Base/String.h"
#include "Swiften/JID/JID.h"
#include "Swiften/Elements/VCard.h"

namespace Swift {
	class XMPPRoster;
	class MUCRegistry;
	class VCardManager;
	class NickResolver {
		public:
			NickResolver(const JID& ownJID, boost::shared_ptr<XMPPRoster> xmppRoster, VCardManager* vcardManager, MUCRegistry* mucRegistry);

			String jidToNick(const JID& jid);
			void setMUCRegistry(MUCRegistry* registry);

			boost::signal<void (const String&)> onOwnNickChanged;
		private:
			void handleVCardReceived(const JID& jid, VCard::ref vCard);
			JID ownJID_;
			String ownNick_;
			std::map<JID, String> map_;

			boost::shared_ptr<XMPPRoster> xmppRoster_;
			MUCRegistry* mucRegistry_;
			VCardManager* vcardManager_;
	};
}
#endif


