/*
 * Copyright (c) 2011-2016 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#include <Swift/Controllers/XMPPURIController.h>

#include <memory>

#include <boost/bind.hpp>

#include <Swift/Controllers/UIEvents/RequestChatUIEvent.h>
#include <Swift/Controllers/UIEvents/RequestJoinMUCUIEvent.h>
#include <Swift/Controllers/UIEvents/UIEventStream.h>

#include <SwifTools/URIHandler/URIHandler.h>
#include <SwifTools/URIHandler/XMPPURI.h>

using namespace Swift;

XMPPURIController::XMPPURIController(URIHandler* uriHandler, UIEventStream* uiEventStream) : uriHandler(uriHandler), uiEventStream(uiEventStream) {
    uriHandler->onURI.connect(boost::bind(&XMPPURIController::handleURI, this, _1));
}

XMPPURIController::~XMPPURIController() {
    uriHandler->onURI.disconnect(boost::bind(&XMPPURIController::handleURI, this, _1));
}

void XMPPURIController::handleURI(const std::string& s) {
    XMPPURI uri = XMPPURI::fromString(s);
    if (!uri.isNull()) {
        if (uri.getQueryType() == "join") {
            uiEventStream->send(std::make_shared<RequestJoinMUCUIEvent>(uri.getPath()));
        }
        else {
            uiEventStream->send(std::make_shared<RequestChatUIEvent>(uri.getPath()));
        }
    }
}
