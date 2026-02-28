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

#include <Swift/Controllers/WhiteboardManager.h>

#include <boost/bind.hpp>

#include <Swiften/Client/NickResolver.h>
#include <Swiften/Client/StanzaChannel.h>
#include <Swiften/Whiteboard/WhiteboardSessionManager.h>

#include <Swift/Controllers/UIEvents/AcceptWhiteboardSessionUIEvent.h>
#include <Swift/Controllers/UIEvents/CancelWhiteboardSessionUIEvent.h>
#include <Swift/Controllers/UIEvents/RequestWhiteboardUIEvent.h>
#include <Swift/Controllers/UIEvents/ShowWhiteboardUIEvent.h>

namespace Swift {
    WhiteboardManager::WhiteboardManager(WhiteboardWindowFactory* whiteboardWindowFactory, UIEventStream* uiEventStream, NickResolver* nickResolver, WhiteboardSessionManager* whiteboardSessionManager) : whiteboardWindowFactory_(whiteboardWindowFactory), uiEventStream_(uiEventStream), nickResolver_(nickResolver), whiteboardSessionManager_(whiteboardSessionManager) {

#ifdef SWIFT_EXPERIMENTAL_WB
        whiteboardSessionManager_->onSessionRequest.connect(boost::bind(&WhiteboardManager::handleIncomingSession, this, _1));
#endif
        uiEventConnection_ = uiEventStream_->onUIEvent.connect(boost::bind(&WhiteboardManager::handleUIEvent, this, _1));
    }

    WhiteboardManager::~WhiteboardManager() {
        for (auto&& whiteboardWindowPair : whiteboardWindows_) {
            delete whiteboardWindowPair.second;
        }
    }

    WhiteboardWindow* WhiteboardManager::createNewWhiteboardWindow(const JID& contact, WhiteboardSession::ref session) {
        WhiteboardWindow *window = whiteboardWindowFactory_->createWhiteboardWindow(session);
        window->setName(nickResolver_->jidToNick(contact));
        whiteboardWindows_[contact.toBare()] = window;
        return window;
    }

    WhiteboardWindow* WhiteboardManager::findWhiteboardWindow(const JID& contact) {
        if (whiteboardWindows_.find(contact.toBare()) == whiteboardWindows_.end()) {
            return nullptr;
        }
        return whiteboardWindows_[contact.toBare()];
    }

    void WhiteboardManager::handleUIEvent(std::shared_ptr<UIEvent> event) {
        std::shared_ptr<RequestWhiteboardUIEvent> requestWhiteboardEvent = std::dynamic_pointer_cast<RequestWhiteboardUIEvent>(event);
        if (requestWhiteboardEvent) {
            requestSession(requestWhiteboardEvent->getContact());
        }
        std::shared_ptr<AcceptWhiteboardSessionUIEvent> sessionAcceptEvent = std::dynamic_pointer_cast<AcceptWhiteboardSessionUIEvent>(event);
        if (sessionAcceptEvent) {
            acceptSession(sessionAcceptEvent->getContact());
        }
        std::shared_ptr<CancelWhiteboardSessionUIEvent> sessionCancelEvent = std::dynamic_pointer_cast<CancelWhiteboardSessionUIEvent>(event);
        if (sessionCancelEvent) {
            cancelSession(sessionCancelEvent->getContact());
        }
        std::shared_ptr<ShowWhiteboardUIEvent> showWindowEvent = std::dynamic_pointer_cast<ShowWhiteboardUIEvent>(event);
        if (showWindowEvent) {
            WhiteboardWindow* window = findWhiteboardWindow(showWindowEvent->getContact());
            if (window != nullptr) {
                window->activateWindow();
            }
        }
    }

    void WhiteboardManager::acceptSession(const JID& from) {
        IncomingWhiteboardSession::ref session = std::dynamic_pointer_cast<IncomingWhiteboardSession>(whiteboardSessionManager_->getSession(from));
        WhiteboardWindow* window = findWhiteboardWindow(from);
        if (session && window) {
            session->accept();
            window->show();
        }
    }

    void WhiteboardManager::requestSession(const JID& contact) {
        WhiteboardSession::ref session = whiteboardSessionManager_->requestSession(contact);
        session->onSessionTerminated.connect(boost::bind(&WhiteboardManager::handleSessionTerminate, this, _1));
        session->onRequestAccepted.connect(boost::bind(&WhiteboardManager::handleSessionAccept, this, _1));
        session->onRequestRejected.connect(boost::bind(&WhiteboardManager::handleRequestReject, this, _1));

        WhiteboardWindow* window = findWhiteboardWindow(contact);
        if (window == nullptr) {
            createNewWhiteboardWindow(contact, session);
        } else {
            window->setSession(session);
        }
        onSessionRequest(session->getTo(), true);
    }

    void WhiteboardManager::cancelSession(const JID& from) {
        WhiteboardSession::ref session = whiteboardSessionManager_->getSession(from);
        if (session) {
            session->cancel();
        }
    }

    void WhiteboardManager::handleIncomingSession(IncomingWhiteboardSession::ref session) {
        session->onSessionTerminated.connect(boost::bind(&WhiteboardManager::handleSessionTerminate, this, _1));
        session->onRequestAccepted.connect(boost::bind(&WhiteboardManager::handleSessionAccept, this, _1));

        WhiteboardWindow* window = findWhiteboardWindow(session->getTo());
        if (window == nullptr) {
            createNewWhiteboardWindow(session->getTo(), session);
        } else {
            window->setSession(session);
        }

        onSessionRequest(session->getTo(), false);
    }

    void WhiteboardManager::handleSessionTerminate(const JID& contact) {
        onSessionTerminate(contact);
    }

    void WhiteboardManager::handleSessionCancel(const JID& contact) {
        onSessionTerminate(contact);
    }

    void WhiteboardManager::handleSessionAccept(const JID& contact) {
        WhiteboardWindow* window = findWhiteboardWindow(contact);
        if (window != nullptr) {
            window->show();
        }
        onRequestAccepted(contact);
    }

    void WhiteboardManager::handleRequestReject(const JID& contact) {
        onRequestRejected(contact);
    }

}
