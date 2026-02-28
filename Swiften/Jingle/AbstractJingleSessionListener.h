/*
 * Copyright (c) 2013-2017 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <Swiften/Base/API.h>
#include <Swiften/Jingle/JingleSessionListener.h>

namespace Swift {
    class SWIFTEN_API AbstractJingleSessionListener : public JingleSessionListener {
        public:
            virtual void handleSessionAcceptReceived(const JingleContentID&, std::shared_ptr<JingleDescription>, std::shared_ptr<JingleTransportPayload>) override;
            virtual void handleSessionInfoReceived(std::shared_ptr<JinglePayload>) override;
            virtual void handleSessionTerminateReceived(boost::optional<JinglePayload::Reason>) override;
            virtual void handleTransportAcceptReceived(const JingleContentID&, std::shared_ptr<JingleTransportPayload>) override;
            virtual void handleTransportInfoReceived(const JingleContentID&, std::shared_ptr<JingleTransportPayload>) override;
            virtual void handleTransportRejectReceived(const JingleContentID&, std::shared_ptr<JingleTransportPayload>) override;
            virtual void handleTransportReplaceReceived(const JingleContentID&, std::shared_ptr<JingleTransportPayload>) override;
            virtual void handleTransportInfoAcknowledged(const std::string& id) override;
    };
}

