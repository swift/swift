#include "Swiften/Server/ServerFromClientSession.h"

#include <boost/bind.hpp>

#include "Swiften/Network/IncomingConnection.h"
#include "Swiften/StreamStack/StreamStack.h"
#include "Swiften/StreamStack/IncomingConnectionLayer.h"
#include "Swiften/StreamStack/XMPPLayer.h"

namespace Swift {

ServerFromClientSession::ServerFromClientSession(
		boost::shared_ptr<IncomingConnection> connection, 
		PayloadParserFactoryCollection* payloadParserFactories, 
		PayloadSerializerCollection* payloadSerializers) : 
			connection_(connection), 
			payloadParserFactories_(payloadParserFactories), 
			payloadSerializers_(payloadSerializers) {
	xmppLayer_ = new XMPPLayer(payloadParserFactories_, payloadSerializers_);
	xmppLayer_->onStreamStart.connect(
			boost::bind(&ServerFromClientSession::handleStreamStart, this, _1));
	xmppLayer_->onElement.connect(
			boost::bind(&ServerFromClientSession::handleElement, this, _1));
	//xmppLayer_->onError.connect(
	//		boost::bind(&ServerFromClientSession::setError, this, XMLError));
	xmppLayer_->onDataRead.connect(
			boost::bind(boost::ref(onDataRead), _1));
	xmppLayer_->onWriteData.connect(
			boost::bind(boost::ref(onDataWritten), _1));
	connectionLayer_ = new IncomingConnectionLayer(connection_.get());
	streamStack_ = new StreamStack(xmppLayer_, connectionLayer_);
}

ServerFromClientSession::~ServerFromClientSession() {
	delete streamStack_;
	delete connectionLayer_;
	delete xmppLayer_;
}

void ServerFromClientSession::handleElement(boost::shared_ptr<Element>) {
}

void ServerFromClientSession::handleStreamStart(const String& domain) {
	xmppLayer_->writeHeader(domain);
}

}
