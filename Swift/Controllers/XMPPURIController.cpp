/*
 * Copyright (c) 2011 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#include <Swift/Controllers/XMPPURIController.h>

#include <boost/bind.hpp>
#include <boost/smart_ptr/make_shared.hpp>

#include <SwifTools/URIHandler/URIHandler.h>
#include <SwifTools/URIHandler/XMPPURI.h>
#include <Swift/Controllers/UIEvents/UIEventStream.h>
#include <Swift/Controllers/UIEvents/RequestJoinMUCUIEvent.h>
#include <Swift/Controllers/UIEvents/RequestChatUIEvent.h>

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
			uiEventStream->send(boost::make_shared<RequestJoinMUCUIEvent>(uri.getPath()));
		}
		else {
			uiEventStream->send(boost::make_shared<RequestChatUIEvent>(uri.getPath()));
		}
	}
}
