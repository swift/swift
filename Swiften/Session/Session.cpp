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
	handleSessionFinished(boost::optional<SessionError>());
	onSessionFinished(boost::optional<SessionError>());
}

void Session::finishSession(const SessionError& error) {
	connection->disconnect();
	handleSessionFinished(boost::optional<SessionError>(error));
	onSessionFinished(boost::optional<SessionError>(error));
}

void Session::initializeStreamStack() {
	xmppLayer = boost::shared_ptr<XMPPLayer>(
			new XMPPLayer(payloadParserFactories, payloadSerializers));
	xmppLayer->onStreamStart.connect(
			boost::bind(&Session::handleStreamStart, shared_from_this(), _1));
	xmppLayer->onElement.connect(boost::bind(&Session::handleElement, shared_from_this(), _1));
	xmppLayer->onError.connect(
			boost::bind(&Session::finishSession, shared_from_this(), XMLError));
	xmppLayer->onDataRead.connect(boost::bind(boost::ref(onDataRead), _1));
	xmppLayer->onWriteData.connect(boost::bind(boost::ref(onDataWritten), _1));
	connection->onDisconnected.connect(
			boost::bind(&Session::handleDisconnected, shared_from_this(), _1));
	connectionLayer = boost::shared_ptr<ConnectionLayer>(new ConnectionLayer(connection));
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

void Session::setInitialized() {
	initialized = true;
	onSessionStarted();
}

}
