// TODO: Send out better errors

#include "Swiften/Session/BasicSessionStream.h"

#include <boost/bind.hpp>

#include "Swiften/StreamStack/XMPPLayer.h"
#include "Swiften/StreamStack/StreamStack.h"
#include "Swiften/StreamStack/ConnectionLayer.h"
#include "Swiften/StreamStack/WhitespacePingLayer.h"
#include "Swiften/StreamStack/TLSLayer.h"
#include "Swiften/StreamStack/TLSLayerFactory.h"

namespace Swift {

BasicSessionStream::BasicSessionStream(boost::shared_ptr<Connection> connection, PayloadParserFactoryCollection* payloadParserFactories, PayloadSerializerCollection* payloadSerializers, TLSLayerFactory* tlsLayerFactory) : available(false), connection(connection), payloadParserFactories(payloadParserFactories), payloadSerializers(payloadSerializers), tlsLayerFactory(tlsLayerFactory) {
}

void BasicSessionStream::initialize() {
	xmppLayer = boost::shared_ptr<XMPPLayer>(
			new XMPPLayer(payloadParserFactories, payloadSerializers));
	xmppLayer->onStreamStart.connect(boost::bind(&BasicSessionStream::handleStreamStartReceived, shared_from_this(), _1));
	xmppLayer->onElement.connect(boost::bind(&BasicSessionStream::handleElementReceived, shared_from_this(), _1));
	xmppLayer->onError.connect(boost::bind(
      &BasicSessionStream::handleXMPPError, shared_from_this()));

	connection->onDisconnected.connect(boost::bind(&BasicSessionStream::handleConnectionError, shared_from_this(), _1));
	connectionLayer = boost::shared_ptr<ConnectionLayer>(
      new ConnectionLayer(connection));

	streamStack = new StreamStack(xmppLayer, connectionLayer);

	available = true;
}

BasicSessionStream::~BasicSessionStream() {
  delete streamStack;
}

void BasicSessionStream::writeHeader(const ProtocolHeader& header) {
	assert(available);
	xmppLayer->writeHeader(header);
}

void BasicSessionStream::writeElement(boost::shared_ptr<Element> element) {
	assert(available);
	xmppLayer->writeElement(element);
}

void BasicSessionStream::writeFooter() {
	assert(available);
	xmppLayer->writeFooter();
}

bool BasicSessionStream::isAvailable() {
	return available;
}

bool BasicSessionStream::supportsTLSEncryption() {
  return tlsLayerFactory && tlsLayerFactory->canCreate();
}

void BasicSessionStream::addTLSEncryption() {
	assert(available);
	tlsLayer = tlsLayerFactory->createTLSLayer();
	if (hasTLSCertificate() && !tlsLayer->setClientCertificate(getTLSCertificate())) {
		onError(boost::shared_ptr<Error>(new Error()));
	}
	else {
		streamStack->addLayer(tlsLayer);
		tlsLayer->onError.connect(boost::bind(&BasicSessionStream::handleTLSError, shared_from_this()));
		tlsLayer->onConnected.connect(boost::bind(&BasicSessionStream::handleTLSConnected, shared_from_this()));
		tlsLayer->connect();
	}
}

void BasicSessionStream::setWhitespacePingEnabled(bool enabled) {
	if (enabled && !whitespacePingLayer) {
		whitespacePingLayer = boost::shared_ptr<WhitespacePingLayer>(new WhitespacePingLayer());
		streamStack->addLayer(whitespacePingLayer);
	}
	if (enabled) {
		whitespacePingLayer->setActive();
	}
	else {
		whitespacePingLayer->setInactive();
	}
}

void BasicSessionStream::resetXMPPParser() {
  xmppLayer->resetParser();
}

void BasicSessionStream::handleStreamStartReceived(const ProtocolHeader& header) {
	onStreamStartReceived(header);
}

void BasicSessionStream::handleElementReceived(boost::shared_ptr<Element> element) {
	onElementReceived(element);
}

void BasicSessionStream::handleXMPPError() {
	available = false;
	onError(boost::shared_ptr<Error>(new Error()));
}

void BasicSessionStream::handleTLSConnected() {
	onTLSEncrypted();
}

void BasicSessionStream::handleTLSError() {
	available = false;
	onError(boost::shared_ptr<Error>(new Error()));
}

void BasicSessionStream::handleConnectionError(const boost::optional<Connection::Error>&) {
	available = false;
	onError(boost::shared_ptr<Error>(new Error()));
}

};
