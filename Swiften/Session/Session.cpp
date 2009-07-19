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
			initialized(false) {
}

Session::~Session() {
	delete streamStack;
}

void Session::startSession() {
	initializeStreamStack();
	handleSessionStarted();
}

void Session::finishSession() {
	connection->disconnect();
	onSessionFinished(boost::optional<Error>());
}

void Session::finishSession(const Error& error) {
	connection->disconnect();
	onSessionFinished(boost::optional<Error>(error));
}

void Session::initializeStreamStack() {
	xmppLayer = boost::shared_ptr<XMPPLayer>(
			new XMPPLayer(payloadParserFactories, payloadSerializers));
	xmppLayer->onStreamStart.connect(
			boost::bind(&Session::handleStreamStart, this, _1));
	xmppLayer->onElement.connect(
			boost::bind(&Session::handleElement, this, _1));
	xmppLayer->onError.connect(
			boost::bind(&Session::finishSession, this, XMLError));
	connection->onDisconnected.connect(
			boost::bind(&Session::handleDisconnected, shared_from_this(), _1));
	connectionLayer = boost::shared_ptr<ConnectionLayer>(new ConnectionLayer(connection));
	streamStack = new StreamStack(xmppLayer, connectionLayer);
}

void Session::sendStanza(boost::shared_ptr<Stanza> stanza) {
	xmppLayer->writeElement(stanza);
}

void Session::handleDisconnected(const boost::optional<Connection::Error>& connectionError) {
	if (connectionError) {
		finishSession(ConnectionError);
	}
	else {
		finishSession();
	}
}

void Session::setInitialized() {
	initialized = true;
	onSessionStarted();
}

}
