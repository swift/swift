/*
 * Copyright (c) 2010 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#pragma once

#include <set>
#include <map>

#include "Swiften/Base/boost_bsignals.h"
#include "Swiften/Elements/Presence.h"
#include "Swiften/Elements/DiscoInfo.h"
#include "Swiften/Elements/CapsInfo.h"
#include "Swiften/Elements/ErrorPayload.h"
#include "Swiften/Disco/CapsProvider.h"

namespace Swift {
	class StanzaChannel;
	class IQRouter;
	class JID;
	class CapsStorage;

	class CapsManager : public CapsProvider, public boost::bsignals::trackable { 
		public:
			CapsManager(CapsStorage*, StanzaChannel*, IQRouter*);

			DiscoInfo::ref getCaps(const String&) const;

			// Mainly for testing purposes
			void setWarnOnInvalidHash(bool b) {
				warnOnInvalidHash = b;
			}

		private:
			void handlePresenceReceived(boost::shared_ptr<Presence>);
			void handleStanzaChannelAvailableChanged(bool);
			void handleDiscoInfoReceived(const JID&, const String& hash, DiscoInfo::ref, const boost::optional<ErrorPayload>&);
			void requestDiscoInfo(const JID& jid, const String& node, const String& hash);

		private:
			IQRouter* iqRouter;
			CapsStorage* capsStorage;
			bool warnOnInvalidHash;
			std::set<String> requestedDiscoInfos;
			std::set< std::pair<JID, String> > failingCaps;
			std::map<String, std::set< std::pair<JID, String> > > fallbacks;
	};
}
