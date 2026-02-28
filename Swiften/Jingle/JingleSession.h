/*
 * Copyright (c) 2011-2016 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <memory>
#include <string>
#include <vector>

#include <boost/optional.hpp>
#include <boost/signals2.hpp>

#include <Swiften/Base/API.h>
#include <Swiften/Base/Listenable.h>
#include <Swiften/Elements/JinglePayload.h>
#include <Swiften/JID/JID.h>

namespace Swift {
    class JingleSessionListener;
    class JingleContentID;

    class SWIFTEN_API JingleSession : public Listenable<JingleSessionListener> {
        public:
            typedef std::shared_ptr<JingleSession> ref;

            JingleSession(const JID& initiator, const std::string& id);
            virtual ~JingleSession();

            const JID& getInitiator() const {
                return initiator;
            }

            const std::string& getID() const {
                return id;
            }

            virtual void sendInitiate(const JingleContentID&, JingleDescription::ref, JingleTransportPayload::ref) = 0;
            virtual void sendTerminate(JinglePayload::Reason::Type reason) = 0;
            virtual void sendInfo(std::shared_ptr<Payload>) = 0;
            virtual void sendAccept(const JingleContentID&, JingleDescription::ref, JingleTransportPayload::ref = JingleTransportPayload::ref()) = 0;
            virtual std::string sendTransportInfo(const JingleContentID&, JingleTransportPayload::ref) = 0;
            virtual void sendTransportAccept(const JingleContentID&, JingleTransportPayload::ref) = 0;
            virtual void sendTransportReject(const JingleContentID&, JingleTransportPayload::ref) = 0;
            virtual void sendTransportReplace(const JingleContentID&, JingleTransportPayload::ref) = 0;

        private:
            JID initiator;
            std::string id;
            std::vector<JingleSessionListener*> listeners;
    };
}
