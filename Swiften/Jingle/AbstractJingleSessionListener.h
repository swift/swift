/*
 * Copyright (c) 2013-2016 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <Swiften/Base/API.h>
#include <Swiften/Base/Override.h>
#include <Swiften/Jingle/JingleSessionListener.h>

namespace Swift {
    class SWIFTEN_API AbstractJingleSessionListener : public JingleSessionListener {
        public:
            virtual void handleSessionAcceptReceived(const JingleContentID&, std::shared_ptr<JingleDescription>, std::shared_ptr<JingleTransportPayload>) SWIFTEN_OVERRIDE;
            virtual void handleSessionInfoReceived(std::shared_ptr<JinglePayload>) SWIFTEN_OVERRIDE;
            virtual void handleSessionTerminateReceived(boost::optional<JinglePayload::Reason>) SWIFTEN_OVERRIDE;
            virtual void handleTransportAcceptReceived(const JingleContentID&, std::shared_ptr<JingleTransportPayload>) SWIFTEN_OVERRIDE;
            virtual void handleTransportInfoReceived(const JingleContentID&, std::shared_ptr<JingleTransportPayload>) SWIFTEN_OVERRIDE;
            virtual void handleTransportRejectReceived(const JingleContentID&, std::shared_ptr<JingleTransportPayload>) SWIFTEN_OVERRIDE;
            virtual void handleTransportReplaceReceived(const JingleContentID&, std::shared_ptr<JingleTransportPayload>) SWIFTEN_OVERRIDE;
            virtual void handleTransportInfoAcknowledged(const std::string& id) SWIFTEN_OVERRIDE;
    };
}

