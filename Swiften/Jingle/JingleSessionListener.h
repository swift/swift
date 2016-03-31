/*
 * Copyright (c) 2013 Isode Limited.
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
                    boost::shared_ptr<JingleDescription>,
                    boost::shared_ptr<JingleTransportPayload>) = 0;
            virtual void handleSessionInfoReceived(boost::shared_ptr<JinglePayload>) = 0;
            virtual void handleSessionTerminateReceived(boost::optional<JinglePayload::Reason>) = 0;
            virtual void handleTransportAcceptReceived(
                    const JingleContentID&,
                    boost::shared_ptr<JingleTransportPayload>) = 0;
            virtual void handleTransportInfoReceived(
                    const JingleContentID&,
                    boost::shared_ptr<JingleTransportPayload>) = 0;
            virtual void handleTransportRejectReceived(
                    const JingleContentID&, boost::shared_ptr<JingleTransportPayload>) = 0;
            virtual void handleTransportReplaceReceived(
                    const JingleContentID&, boost::shared_ptr<JingleTransportPayload>) = 0;

            virtual void handleTransportInfoAcknowledged(const std::string& id) = 0;
    };
}
