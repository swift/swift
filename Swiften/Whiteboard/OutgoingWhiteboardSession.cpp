/*
 * Copyright (c) 2012 Mateusz Piękos
 * Licensed under the simplified BSD license.
 * See Documentation/Licenses/BSD-simplified.txt for more information.
 */

#include <Swiften/Whiteboard/OutgoingWhiteboardSession.h>

#include <boost/smart_ptr/make_shared.hpp>
#include <boost/bind.hpp>
#include <Swiften/Elements/WhiteboardPayload.h>

#include <Swiften/Elements/Whiteboard/WhiteboardInsertOperation.h>
#include <Swiften/Elements/Whiteboard/WhiteboardUpdateOperation.h>
#include <Swiften/Elements/Whiteboard/WhiteboardDeleteOperation.h>

namespace Swift {
	OutgoingWhiteboardSession::OutgoingWhiteboardSession(const JID& jid, IQRouter* router) : WhiteboardSession(jid, router) {
	}

	OutgoingWhiteboardSession::~OutgoingWhiteboardSession() {
	}

	void OutgoingWhiteboardSession::startSession() {
		boost::shared_ptr<WhiteboardPayload> payload = boost::make_shared<WhiteboardPayload>(WhiteboardPayload::SessionRequest);
		boost::shared_ptr<GenericRequest<WhiteboardPayload> > request = boost::make_shared<GenericRequest<WhiteboardPayload> >(IQ::Set, toJID_, payload, router_);
		request->onResponse.connect(boost::bind(&OutgoingWhiteboardSession::handleRequestResponse, this, _1, _2));
		request->send();
	}

	void OutgoingWhiteboardSession::handleRequestResponse(boost::shared_ptr<WhiteboardPayload> /*payload*/, ErrorPayload::ref error) {
		if (error) {
			onRequestRejected(toJID_);
		}
	}

	void OutgoingWhiteboardSession::handleIncomingOperation(WhiteboardOperation::ref operation) {
		WhiteboardOperation::ref op = server.handleClientOperationReceived(operation);
		if (op->getPos() != -1) {
			onOperationReceived(op);
		}
		lastOpID = op->getID();

		WhiteboardPayload::ref payload = boost::make_shared<WhiteboardPayload>();
		payload->setOperation(op);
		sendPayload(payload);
	}

	void OutgoingWhiteboardSession::sendOperation(WhiteboardOperation::ref operation) {
		operation->setID(idGenerator.generateID());
		operation->setParentID(lastOpID);
		lastOpID = operation->getID();

		server.handleLocalOperationReceived(operation);
		WhiteboardPayload::ref payload = boost::make_shared<WhiteboardPayload>();
		payload->setOperation(operation);
		sendPayload(payload);
	}
}
