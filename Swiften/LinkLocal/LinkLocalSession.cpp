#include "Swiften/LinkLocal/LinkLocalSession.h"

#include <boost/bind.hpp>

#include "Swiften/StreamStack/XMPPLayer.h"
#include "Swiften/StreamStack/StreamStack.h"

namespace Swift {

LinkLocalSession::LinkLocalSession(
		const JID& localJID,
		boost::shared_ptr<Connection> connection,
		PayloadParserFactoryCollection* payloadParserFactories, 
		PayloadSerializerCollection* payloadSerializers) : 
			localJID(localJID),
			payloadParserFactories(payloadParserFactories),
			payloadSerializers(payloadSerializers),
			connection(connection),
			initialized(false) {
}

LinkLocalSession::~LinkLocalSession() {
	delete streamStack;
}

void LinkLocalSession::initializeStreamStack() {
	assert(connection);
	xmppLayer = boost::shared_ptr<XMPPLayer>(
			new XMPPLayer(payloadParserFactories, payloadSerializers));
	xmppLayer->onStreamStart.connect(
			boost::bind(&LinkLocalSession::handleStreamStart, this, _1));
	xmppLayer->onElement.connect(
			boost::bind(&LinkLocalSession::handleElement, this, _1));
	//xmppLayer->onError.connect(
	//		boost::bind(&LinkLocalSession::setError, this, XMLError));
	connection->onDisconnected.connect(
			boost::bind(&LinkLocalSession::handleDisconnected, shared_from_this(), _1));
	connectionLayer = boost::shared_ptr<ConnectionLayer>(new ConnectionLayer(connection));
	streamStack = new StreamStack(xmppLayer, connectionLayer);
}

void LinkLocalSession::finishSession() {
	connection->disconnect();
}

void LinkLocalSession::sendStanza(boost::shared_ptr<Stanza> stanza) {
	xmppLayer->writeElement(stanza);
}

void LinkLocalSession::handleDisconnected(const boost::optional<Connection::Error>& connectionError) {
	if (connectionError) {
		onSessionFinished(boost::optional<Error>(ConnectionError));
	}
	else {
		onSessionFinished(boost::optional<Error>());
	}
}

void LinkLocalSession::setInitialized() {
	initialized = true;
	onSessionStarted();
}

}
