/*
 * Copyright (c) 2010 Kevin Smith
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#pragma once

#include <boost/shared_ptr.hpp>

#include "Swiften/JID/JID.h"
#include <string>
#include "Swiften/Elements/IQ.h"
#include "Swiften/Elements/RosterPayload.h"
#include "Swiften/Roster/RosterPushResponder.h"
#include "Swiften/Base/boost_bsignals.h"

namespace Swift {
	class IQRouter;
	class XMPPRosterImpl;

	class XMPPRosterController {
		public:
			XMPPRosterController(IQRouter *iqRouter, XMPPRosterImpl* xmppRoster);
			~XMPPRosterController();

			void requestRoster();

		private:
			void handleRosterReceived(boost::shared_ptr<RosterPayload> rosterPayload, bool initial);

		private:
			IQRouter* iqRouter_;
			RosterPushResponder rosterPushResponder_;
			XMPPRosterImpl* xmppRoster_;
	};
}

