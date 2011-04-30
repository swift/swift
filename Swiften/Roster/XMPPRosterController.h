/*
 * Copyright (c) 2010 Kevin Smith
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#pragma once

#include <boost/shared_ptr.hpp>

#include <Swiften/JID/JID.h>
#include <string>
#include <Swiften/Elements/IQ.h>
#include <Swiften/Elements/RosterPayload.h>
#include <Swiften/Roster/RosterPushResponder.h>
#include <Swiften/Base/boost_bsignals.h>

namespace Swift {
	class IQRouter;
	class XMPPRosterImpl;
	class RosterStorage;

	class XMPPRosterController {
		public:
			XMPPRosterController(IQRouter *iqRouter, XMPPRosterImpl* xmppRoster, RosterStorage* storage);
			~XMPPRosterController();

			void requestRoster();

			void setUseVersioning(bool b) {
				useVersioning = b;
			}

		private:
			void handleRosterReceived(boost::shared_ptr<RosterPayload> rosterPayload, bool initial, boost::shared_ptr<RosterPayload> previousRoster);
			void saveRoster(const std::string& version);

		private:
			IQRouter* iqRouter_;
			RosterPushResponder rosterPushResponder_;
			XMPPRosterImpl* xmppRoster_;
			RosterStorage* rosterStorage_;
			bool useVersioning;
	};
}

