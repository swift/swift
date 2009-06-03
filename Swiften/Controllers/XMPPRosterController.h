#pragma once

#include "Swiften/JID/JID.h"
#include "Swiften/Base/String.h"
#include "Swiften/Elements/IQ.h"
#include "Swiften/Elements/RosterPayload.h"
#include "Swiften/Queries/IQHandler.h"

#include <boost/signals.hpp>
#include <boost/shared_ptr.hpp>

namespace Swift {
	class IQRouter;
	class XMPPRoster;

	class XMPPRosterController : public IQHandler {
		public:
			XMPPRosterController(IQRouter *iqRouter, boost::shared_ptr<XMPPRoster> xmppRoster);
			~XMPPRosterController();

			void requestRoster();

			boost::shared_ptr<XMPPRoster> getXMPPRoster() {return xmppRoster_;};
			bool handleIQ(boost::shared_ptr<IQ>);

		private:
			IQRouter* iqRouter_;
			void handleRosterReceived(boost::shared_ptr<RosterPayload> rosterPayload);
			boost::shared_ptr<XMPPRoster> xmppRoster_;
	};
}

