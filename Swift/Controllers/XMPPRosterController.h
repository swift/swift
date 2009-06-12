#pragma once

#include "Swiften/JID/JID.h"
#include "Swiften/Base/String.h"
#include "Swiften/Elements/IQ.h"
#include "Swiften/Elements/RosterPayload.h"
#include "Swiften/Queries/Responders/RosterPushResponder.h"

#include <boost/signals.hpp>
#include <boost/shared_ptr.hpp>

namespace Swift {
	class IQRouter;
	class XMPPRoster;

	class XMPPRosterController {
		public:
			XMPPRosterController(IQRouter *iqRouter, boost::shared_ptr<XMPPRoster> xmppRoster);

			void requestRoster();

			boost::shared_ptr<XMPPRoster> getXMPPRoster() {return xmppRoster_;};

			void handleRosterReceived(boost::shared_ptr<RosterPayload> rosterPayload);

		private:
			IQRouter* iqRouter_;
			RosterPushResponder rosterPushResponder_;
			boost::shared_ptr<XMPPRoster> xmppRoster_;
	};
}

