/*
 * Copyright (c) 2010 Kevin Smith
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#pragma once

#include <boost/shared_ptr.hpp>

#include "Swiften/JID/JID.h"
#include "Swiften/Base/String.h"
#include "Swiften/Elements/IQ.h"
#include "Swiften/Elements/RosterPayload.h"
#include "Swiften/Queries/Responders/RosterPushResponder.h"
#include "Swiften/Base/boost_bsignals.h"

namespace Swift {
	class IQRouter;
	class XMPPRoster;

	class XMPPRosterController {
		public:
			XMPPRosterController(IQRouter *iqRouter, XMPPRoster* xmppRoster);
			~XMPPRosterController();

			void requestRoster();

			void handleRosterReceived(boost::shared_ptr<RosterPayload> rosterPayload);

		private:
			IQRouter* iqRouter_;
			RosterPushResponder rosterPushResponder_;
			XMPPRoster* xmppRoster_;
	};
}

