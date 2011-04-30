/*
 * Copyright (c) 2010 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#pragma once

#include <set>
#include <map>

#include <Swiften/Base/boost_bsignals.h>
#include <Swiften/Elements/Presence.h>
#include <Swiften/Elements/DiscoInfo.h>
#include <Swiften/Elements/CapsInfo.h>
#include <Swiften/Elements/ErrorPayload.h>
#include <Swiften/Disco/CapsProvider.h>

namespace Swift {
	class StanzaChannel;
	class IQRouter;
	class JID;
	class CapsStorage;

	class CapsManager : public CapsProvider, public boost::bsignals::trackable { 
		public:
			CapsManager(CapsStorage*, StanzaChannel*, IQRouter*);

			DiscoInfo::ref getCaps(const std::string&) const;

			// Mainly for testing purposes
			void setWarnOnInvalidHash(bool b) {
				warnOnInvalidHash = b;
			}

		private:
			void handlePresenceReceived(boost::shared_ptr<Presence>);
			void handleStanzaChannelAvailableChanged(bool);
			void handleDiscoInfoReceived(const JID&, const std::string& hash, DiscoInfo::ref, ErrorPayload::ref);
			void requestDiscoInfo(const JID& jid, const std::string& node, const std::string& hash);

		private:
			IQRouter* iqRouter;
			CapsStorage* capsStorage;
			bool warnOnInvalidHash;
			std::set<std::string> requestedDiscoInfos;
			std::set< std::pair<JID, std::string> > failingCaps;
			std::map<std::string, std::set< std::pair<JID, std::string> > > fallbacks;
	};
}
