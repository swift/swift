/*
 * Copyright (c) 2013-2016 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <Swiften/Base/API.h>
#include <Swiften/Elements/JinglePayload.h>

namespace Swift {
    class JingleContentID;
    class JingleTransportPayload;
    class JingleDescription;

    class SWIFTEN_API JingleSessionListener {
        public:
            virtual ~JingleSessionListener();

            virtual void handleSessionAcceptReceived(
                    const JingleContentID&,
                    std::shared_ptr<JingleDescription>,
                    std::shared_ptr<JingleTransportPayload>) = 0;
            virtual void handleSessionInfoReceived(std::shared_ptr<JinglePayload>) = 0;
            virtual void handleSessionTerminateReceived(boost::optional<JinglePayload::Reason>) = 0;
            virtual void handleTransportAcceptReceived(
                    const JingleContentID&,
                    std::shared_ptr<JingleTransportPayload>) = 0;
            virtual void handleTransportInfoReceived(
                    const JingleContentID&,
                    std::shared_ptr<JingleTransportPayload>) = 0;
            virtual void handleTransportRejectReceived(
                    const JingleContentID&, std::shared_ptr<JingleTransportPayload>) = 0;
            virtual void handleTransportReplaceReceived(
                    const JingleContentID&, std::shared_ptr<JingleTransportPayload>) = 0;

            virtual void handleTransportInfoAcknowledged(const std::string& id) = 0;
    };
}
