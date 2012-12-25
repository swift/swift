/*
 * Copyright (c) 2013 Remko Tronçon
 * Licensed under the GNU General Public License.
 * See the COPYING file for more information.
 */

#pragma once

#include <Swiften/Base/API.h>
#include <Swiften/Base/Override.h>
#include <Swiften/Jingle/JingleSessionListener.h>

namespace Swift {
	class SWIFTEN_API AbstractJingleSessionListener : public JingleSessionListener {
		public:
			virtual void handleSessionAcceptReceived(const JingleContentID&, boost::shared_ptr<JingleDescription>, boost::shared_ptr<JingleTransportPayload>) SWIFTEN_OVERRIDE;
			virtual void handleSessionInfoReceived(boost::shared_ptr<JinglePayload>) SWIFTEN_OVERRIDE;
			virtual void handleSessionTerminateReceived(boost::optional<JinglePayload::Reason>) SWIFTEN_OVERRIDE;
			virtual void handleTransportAcceptReceived(const JingleContentID&, boost::shared_ptr<JingleTransportPayload>) SWIFTEN_OVERRIDE;
			virtual void handleTransportInfoReceived(const JingleContentID&, boost::shared_ptr<JingleTransportPayload>) SWIFTEN_OVERRIDE;
			virtual void handleTransportRejectReceived(const JingleContentID&, boost::shared_ptr<JingleTransportPayload>) SWIFTEN_OVERRIDE;
			virtual void handleTransportReplaceReceived(const JingleContentID&, boost::shared_ptr<JingleTransportPayload>) SWIFTEN_OVERRIDE;
			virtual void handleTransportInfoAcknowledged(const std::string& id) SWIFTEN_OVERRIDE;
	};
}

