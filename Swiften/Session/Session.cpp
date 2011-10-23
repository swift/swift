/*
 * Copyright (c) 2010 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#include <Swiften/Session/Session.h>

#include <boost/bind.hpp>

#include <Swiften/StreamStack/XMPPLayer.h>
#include <Swiften/StreamStack/StreamStack.h>

namespace Swift {

Session::Session(
		boost::shared_ptr<Connection> connection,
		PayloadParserFactoryCollection* payloadParserFactories, 
		PayloadSerializerCollection* payloadSerializers,
		XMLParserFactory* xmlParserFactory) : 
			connection(connection),
			payloadParserFactories(payloadParserFactories),
			payloadSerializers(payloadSerializers),
			xmlParserFactory(xmlParserFactory),
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
	if (finishing) {
		return;
	}
	finishing = true;
	if (xmppLayer) {
		xmppLayer->writeFooter();
	}
	connection->disconnect();
}

void Session::finishSession(const SessionError& error) {
	if (finishing) {
		return;
	}
	finishing = true;
	if (xmppLayer) {
		xmppLayer->writeFooter();
	}
	connection->disconnect();
}

void Session::initializeStreamStack() {
	xmppLayer = new XMPPLayer(payloadParserFactories, payloadSerializers, xmlParserFactory, ClientStreamType);
	xmppLayer->onStreamStart.connect(
			boost::bind(&Session::handleStreamStart, this, _1));
	xmppLayer->onElement.connect(boost::bind(&Session::handleElement, this, _1));
	xmppLayer->onError.connect(
			boost::bind(&Session::finishSession, this, XMLError));
	xmppLayer->onDataRead.connect(boost::bind(boost::ref(onDataRead), _1));
	xmppLayer->onWriteData.connect(boost::bind(boost::ref(onDataWritten), _1));
	connection->onDisconnected.connect(
			boost::bind(&Session::handleDisconnected, this, _1));
	connectionLayer = new ConnectionLayer(connection);
	streamStack = new StreamStack(xmppLayer, connectionLayer);
}

void Session::sendElement(boost::shared_ptr<Element> stanza) {
	xmppLayer->writeElement(stanza);
}

void Session::handleDisconnected(const boost::optional<Connection::Error>& connectionError) {
	connection->onDisconnected.disconnect(
			boost::bind(&Session::handleDisconnected, this, _1));
	if (connectionError) {
		switch (*connectionError) {
			case Connection::ReadError:
				handleSessionFinished(ConnectionReadError);
				onSessionFinished(ConnectionReadError);
				break;
			case Connection::WriteError:
				handleSessionFinished(ConnectionWriteError);
				onSessionFinished(ConnectionWriteError);
				break;
		}
	}
	else {
		boost::optional<SessionError> error;
		handleSessionFinished(error);
		onSessionFinished(error);
	}
}

}
