// TODO: whitespacePingLayer_->setInactive();

#include "Swiften/Session/BasicSessionStream.h"

#include <boost/bind.hpp>

#include "Swiften/StreamStack/XMPPLayer.h"
#include "Swiften/StreamStack/StreamStack.h"
#include "Swiften/StreamStack/ConnectionLayer.h"
#include "Swiften/StreamStack/WhitespacePingLayer.h"
#include "Swiften/StreamStack/TLSLayer.h"
#include "Swiften/StreamStack/TLSLayerFactory.h"

namespace Swift {

BasicSessionStream::BasicSessionStream(boost::shared_ptr<Connection> connection, PayloadParserFactoryCollection* payloadParserFactories, PayloadSerializerCollection* payloadSerializers, TLSLayerFactory* tlsLayerFactory) : connection(connection), payloadParserFactories(payloadParserFactories), payloadSerializers(payloadSerializers), tlsLayerFactory(tlsLayerFactory) {
}

void BasicSessionStream::initialize() {
	xmppLayer = boost::shared_ptr<XMPPLayer>(
			new XMPPLayer(payloadParserFactories, payloadSerializers));
	xmppLayer->onStreamStart.connect(boost::bind(&BasicSessionStream::handleStreamStartReceived, shared_from_this(), _1));
	xmppLayer->onElement.connect(boost::bind(&BasicSessionStream::handleElementReceived, shared_from_this(), _1));
	xmppLayer->onError.connect(boost::bind(
      &BasicSessionStream::handleXMPPError, shared_from_this()));

	connectionLayer = boost::shared_ptr<ConnectionLayer>(
      new ConnectionLayer(connection));

	streamStack = new StreamStack(xmppLayer, connectionLayer);
}

BasicSessionStream::~BasicSessionStream() {
  delete streamStack;
}

void BasicSessionStream::writeHeader(const ProtocolHeader& header) {
	xmppLayer->writeHeader(header);
}

void BasicSessionStream::writeElement(boost::shared_ptr<Element> element) {
	xmppLayer->writeElement(element);
}

bool BasicSessionStream::supportsTLSEncryption() {
  return tlsLayerFactory && tlsLayerFactory->canCreate();
}

void BasicSessionStream::addTLSEncryption() {
	tlsLayer = tlsLayerFactory->createTLSLayer();
  streamStack->addLayer(tlsLayer);
  // TODO: Add tls layer certificate if needed
  tlsLayer->onError.connect(boost::bind(&BasicSessionStream::handleTLSError, shared_from_this()));
  tlsLayer->connect();
}

void BasicSessionStream::addWhitespacePing() {
  whitespacePingLayer = boost::shared_ptr<WhitespacePingLayer>(new WhitespacePingLayer());
  streamStack->addLayer(whitespacePingLayer);
  whitespacePingLayer->setActive();
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
	onError(boost::shared_ptr<Error>(new Error()));
}

void BasicSessionStream::handleTLSError() {
	onError(boost::shared_ptr<Error>(new Error()));
}

};
