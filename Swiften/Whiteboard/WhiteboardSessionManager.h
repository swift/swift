/*
 * Copyright (c) 2012 Mateusz Piękos
 * Licensed under the simplified BSD license.
 * See Documentation/Licenses/BSD-simplified.txt for more information.
 */

/*
 * Copyright (c) 2016 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <map>

#include <boost/signals2.hpp>

#include <Swiften/Base/API.h>
#include <Swiften/Client/StanzaChannel.h>
#include <Swiften/JID/JID.h>
#include <Swiften/Queries/IQRouter.h>
#include <Swiften/Whiteboard/IncomingWhiteboardSession.h>
#include <Swiften/Whiteboard/OutgoingWhiteboardSession.h>
#include <Swiften/Whiteboard/WhiteboardSession.h>

namespace Swift {
    class IQRouter;
    class WhiteboardResponder;
    class PresenceOracle;
    class EntityCapsProvider;

    class SWIFTEN_API WhiteboardSessionManager {
        friend class WhiteboardResponder;
    public:
        WhiteboardSessionManager(IQRouter* router, StanzaChannel* stanzaChannel, PresenceOracle* presenceOracle, EntityCapsProvider* capsProvider);
        ~WhiteboardSessionManager();

        WhiteboardSession::ref getSession(const JID& to);
        WhiteboardSession::ref requestSession(const JID& to);

    public:
        boost::signals2::signal< void (IncomingWhiteboardSession::ref)> onSessionRequest;

    private:
        JID getFullJID(const JID& bareJID);
        OutgoingWhiteboardSession::ref createOutgoingSession(const JID& to);
        void handleIncomingSession(IncomingWhiteboardSession::ref session);
        void handlePresenceReceived(Presence::ref presence);
        void handleAvailableChanged(bool available);
        void deleteSessionEntry(const JID& contact);

    private:
        std::map<JID, std::shared_ptr<WhiteboardSession> > sessions_;
        IQRouter* router_;
        StanzaChannel* stanzaChannel_;
        PresenceOracle* presenceOracle_;
        EntityCapsProvider* capsProvider_;
        WhiteboardResponder* responder;
    };
}
