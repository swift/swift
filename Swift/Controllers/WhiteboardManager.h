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
#include <memory>

#include <Swiften/JID/JID.h>
#include <Swiften/Whiteboard/IncomingWhiteboardSession.h>
#include <Swiften/Whiteboard/WhiteboardSession.h>

#include <Swift/Controllers/UIEvents/UIEventStream.h>
#include <Swift/Controllers/UIInterfaces/WhiteboardWindow.h>
#include <Swift/Controllers/UIInterfaces/WhiteboardWindowFactory.h>

namespace Swift {
    class WhiteboardSessionManager;
    class NickResolver;

    class WhiteboardManager {
    public:
        WhiteboardManager(WhiteboardWindowFactory* whiteboardWindowFactory, UIEventStream* uiEventStream, NickResolver* nickResolver, WhiteboardSessionManager* whiteboardSessionManager);
        ~WhiteboardManager();

        WhiteboardWindow* createNewWhiteboardWindow(const JID& contact, WhiteboardSession::ref session);

    public:
        boost::signals2::signal< void (const JID&, bool senderIsSelf)> onSessionRequest;
        boost::signals2::signal< void (const JID&)> onSessionTerminate;
        boost::signals2::signal< void (const JID&)> onRequestAccepted;
        boost::signals2::signal< void (const JID&)> onRequestRejected;

    private:
        void handleUIEvent(std::shared_ptr<UIEvent> event);
        void handleSessionTerminate(const JID& contact);
        void handleSessionCancel(const JID& contact);
        void handleSessionAccept(const JID& contact);
        void handleRequestReject(const JID& contact);
        void handleIncomingSession(IncomingWhiteboardSession::ref session);
        void acceptSession(const JID& from);
        void requestSession(const JID& contact);
        void cancelSession(const JID& from);
        WhiteboardWindow* findWhiteboardWindow(const JID& contact);

    private:
        std::map<JID, WhiteboardWindow*> whiteboardWindows_;
        WhiteboardWindowFactory* whiteboardWindowFactory_;
        UIEventStream* uiEventStream_;
        NickResolver* nickResolver_;
        boost::signals2::scoped_connection uiEventConnection_;
        WhiteboardSessionManager* whiteboardSessionManager_;
    };
}
