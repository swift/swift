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

BasicSessionStream::BasicSessionStream(boost::shared_ptr<Connection> connection, PayloadParserFactoryCollection* payloadParserFactories, PayloadSerializerCollection* payloadSerializers, TLSLayerFactory* tlsLayerFactory) : tlsLayerFactory(tlsLayerFactory) {
	xmppLayer = boost::shared_ptr<XMPPLayer>(
			new XMPPLayer(payloadParserFactories, payloadSerializers));
	xmppLayer->onStreamStart.connect(boost::ref(onStreamStartReceived));
	xmppLayer->onElement.connect(boost::ref(onElementReceived));
	xmppLayer->onError.connect(boost::bind(
      &BasicSessionStream::handleXMPPError, this));

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
  tlsLayer->onError.connect(boost::bind(&BasicSessionStream::handleTLSError, this));
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

void BasicSessionStream::handleXMPPError() {
  // TODO
}

void BasicSessionStream::handleTLSError() {
  // TODO
}

};
