/*
 * Copyright (c) 2010 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#include "Swiften/Session/Session.h"

#include <boost/bind.hpp>

#include "Swiften/StreamStack/XMPPLayer.h"
#include "Swiften/StreamStack/StreamStack.h"

namespace Swift {

Session::Session(
		boost::shared_ptr<Connection> connection,
		PayloadParserFactoryCollection* payloadParserFactories, 
		PayloadSerializerCollection* payloadSerializers) : 
			connection(connection),
			payloadParserFactories(payloadParserFactories),
			payloadSerializers(payloadSerializers),
			xmppLayer(NULL),
			connectionLayer(NULL),
			streamStack(0),
			finishing(false) {
}

Session::~Session() {
	delete streamStack;
	delete connectionLayer;
	delete xmppLayer;
}

void Session::startSession() {
	initializeStreamStack();
	handleSessionStarted();
}

void Session::finishSession() {
	finishing = true;
	connection->disconnect();
	handleSessionFinished(boost::optional<SessionError>());
	finishing = false;
	onSessionFinished(boost::optional<SessionError>());
}

void Session::finishSession(const SessionError& error) {
	finishing = true;
	connection->disconnect();
	handleSessionFinished(boost::optional<SessionError>(error));
	finishing = false;
	onSessionFinished(boost::optional<SessionError>(error));
}

void Session::initializeStreamStack() {
	xmppLayer = new XMPPLayer(payloadParserFactories, payloadSerializers, ClientStreamType);
	xmppLayer->onStreamStart.connect(
			boost::bind(&Session::handleStreamStart, shared_from_this(), _1));
	xmppLayer->onElement.connect(boost::bind(&Session::handleElement, shared_from_this(), _1));
	xmppLayer->onError.connect(
			boost::bind(&Session::finishSession, shared_from_this(), XMLError));
	xmppLayer->onDataRead.connect(boost::bind(boost::ref(onDataRead), _1));
	xmppLayer->onWriteData.connect(boost::bind(boost::ref(onDataWritten), _1));
	connection->onDisconnected.connect(
			boost::bind(&Session::handleDisconnected, shared_from_this(), _1));
	connectionLayer = new ConnectionLayer(connection);
	streamStack = new StreamStack(xmppLayer, connectionLayer);
}

void Session::sendElement(boost::shared_ptr<Element> stanza) {
	xmppLayer->writeElement(stanza);
}

void Session::handleDisconnected(const boost::optional<Connection::Error>& connectionError) {
	if (connectionError) {
		switch (*connectionError) {
			case Connection::ReadError:
				finishSession(ConnectionReadError);
				break;
			case Connection::WriteError:
				finishSession(ConnectionWriteError);
				break;
		}
	}
	else {
		finishSession();
	}
}

}
