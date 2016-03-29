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

#include <Swiften/Whiteboard/WhiteboardSession.h>

#include <iostream>

#include <boost/smart_ptr/make_shared.hpp>

#include <Swiften/Elements/ErrorPayload.h>
#include <Swiften/Elements/WhiteboardPayload.h>
#include <Swiften/Queries/IQRouter.h>

namespace Swift {
	WhiteboardSession::WhiteboardSession(const JID& jid, IQRouter* router) : toJID_(jid), router_(router) {
	}

	WhiteboardSession::~WhiteboardSession() {
	}

	void WhiteboardSession::handleIncomingAction(boost::shared_ptr<WhiteboardPayload> payload) {
		switch (payload->getType()) {
			case WhiteboardPayload::Data:
				handleIncomingOperation(payload->getOperation());
				return;
			case WhiteboardPayload::SessionAccept:
				onRequestAccepted(toJID_);
				return;
			case WhiteboardPayload::SessionTerminate:
				onSessionTerminated(toJID_);
				return;

			//handled elsewhere
			case WhiteboardPayload::SessionRequest:

			case WhiteboardPayload::UnknownType:
				return;
		}
	}

	void WhiteboardSession::sendElement(const WhiteboardElement::ref element) {
		boost::shared_ptr<WhiteboardPayload> payload = boost::make_shared<WhiteboardPayload>();
		payload->setElement(element);
		boost::shared_ptr<GenericRequest<WhiteboardPayload> > request = boost::make_shared<GenericRequest<WhiteboardPayload> >(IQ::Set, toJID_, payload, router_);
		request->send();
	}

	void WhiteboardSession::sendPayload(boost::shared_ptr<WhiteboardPayload> payload) {
		boost::shared_ptr<GenericRequest<WhiteboardPayload> > request = boost::make_shared<GenericRequest<WhiteboardPayload> >(IQ::Set, toJID_, payload, router_);
		request->send();
	}

	void WhiteboardSession::cancel() {
		if (router_->isAvailable()) {
			boost::shared_ptr<WhiteboardPayload> payload = boost::make_shared<WhiteboardPayload>(WhiteboardPayload::SessionTerminate);
			boost::shared_ptr<GenericRequest<WhiteboardPayload> > request = boost::make_shared<GenericRequest<WhiteboardPayload> >(IQ::Set, toJID_, payload, router_);
			request->send();
		}
		onSessionTerminated(toJID_);
	}

	const JID& WhiteboardSession::getTo() const {
		return toJID_;
	}
}
