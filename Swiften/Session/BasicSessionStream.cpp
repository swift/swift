/*
 * Copyright (c) 2010 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#include "Swiften/Session/BasicSessionStream.h"

#include <boost/bind.hpp>

#include "Swiften/StreamStack/XMPPLayer.h"
#include "Swiften/StreamStack/StreamStack.h"
#include "Swiften/StreamStack/ConnectionLayer.h"
#include "Swiften/StreamStack/WhitespacePingLayer.h"
#include "Swiften/StreamStack/CompressionLayer.h"
#include "Swiften/StreamStack/TLSLayer.h"
#include "Swiften/TLS/TLSContextFactory.h"
#include "Swiften/TLS/TLSContext.h"

namespace Swift {

BasicSessionStream::BasicSessionStream(StreamType streamType, boost::shared_ptr<Connection> connection, PayloadParserFactoryCollection* payloadParserFactories, PayloadSerializerCollection* payloadSerializers, TLSContextFactory* tlsContextFactory, TimerFactory* timerFactory) : available(false), connection(connection), payloadParserFactories(payloadParserFactories), payloadSerializers(payloadSerializers), tlsContextFactory(tlsContextFactory), timerFactory(timerFactory), streamType(streamType) {
}

void BasicSessionStream::initialize() {
	xmppLayer = boost::shared_ptr<XMPPLayer>(
			new XMPPLayer(payloadParserFactories, payloadSerializers, streamType));
	xmppLayer->onStreamStart.connect(boost::bind(&BasicSessionStream::handleStreamStartReceived, shared_from_this(), _1));
	xmppLayer->onElement.connect(boost::bind(&BasicSessionStream::handleElementReceived, shared_from_this(), _1));
	xmppLayer->onError.connect(boost::bind(
			&BasicSessionStream::handleXMPPError, shared_from_this()));
	xmppLayer->onDataRead.connect(boost::bind(&BasicSessionStream::handleDataRead, shared_from_this(), _1));
	xmppLayer->onWriteData.connect(boost::bind(&BasicSessionStream::handleDataWritten, shared_from_this(), _1));

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
	return tlsContextFactory && tlsContextFactory->canCreate();
}

void BasicSessionStream::addTLSEncryption() {
	assert(available);
	tlsLayer = boost::shared_ptr<TLSLayer>(new TLSLayer(tlsContextFactory));
	if (hasTLSCertificate() && !tlsLayer->setClientCertificate(getTLSCertificate())) {
		onError(boost::shared_ptr<Error>(new Error(Error::InvalidTLSCertificateError)));
	}
	else {
		streamStack->addLayer(tlsLayer);
		tlsLayer->onError.connect(boost::bind(&BasicSessionStream::handleTLSError, shared_from_this()));
		tlsLayer->onConnected.connect(boost::bind(&BasicSessionStream::handleTLSConnected, shared_from_this()));
		tlsLayer->connect();
	}
}

bool BasicSessionStream::isTLSEncrypted() {
	return tlsLayer;
}

Certificate::ref BasicSessionStream::getPeerCertificate() const {
	return tlsLayer->getPeerCertificate();
}

boost::shared_ptr<CertificateVerificationError> BasicSessionStream::getPeerCertificateVerificationError() const {
	return tlsLayer->getPeerCertificateVerificationError();
}

ByteArray BasicSessionStream::getTLSFinishMessage() const {
	return tlsLayer->getContext()->getFinishMessage();
}

void BasicSessionStream::addZLibCompression() {
	boost::shared_ptr<CompressionLayer> compressionLayer(new CompressionLayer());
	streamStack->addLayer(compressionLayer);
}

void BasicSessionStream::setWhitespacePingEnabled(bool enabled) {
	if (enabled) {
		if (!whitespacePingLayer) {
			whitespacePingLayer = boost::shared_ptr<WhitespacePingLayer>(new WhitespacePingLayer(timerFactory));
			streamStack->addLayer(whitespacePingLayer);
		}
		whitespacePingLayer->setActive();
	}
	else if (whitespacePingLayer) {
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
	onError(boost::shared_ptr<Error>(new Error(Error::ParseError)));
}

void BasicSessionStream::handleTLSConnected() {
	onTLSEncrypted();
}

void BasicSessionStream::handleTLSError() {
	available = false;
	onError(boost::shared_ptr<Error>(new Error(Error::TLSError)));
}

void BasicSessionStream::handleConnectionError(const boost::optional<Connection::Error>& error) {
	available = false;
	if (error == Connection::ReadError) {
		onError(boost::shared_ptr<Error>(new Error(Error::ConnectionReadError)));
	}
	else {
		onError(boost::shared_ptr<Error>(new Error(Error::ConnectionWriteError)));
	}
}

void BasicSessionStream::handleDataRead(const ByteArray& data) {
	onDataRead(String(data.getData(), data.getSize()));
}

void BasicSessionStream::handleDataWritten(const ByteArray& data) {
	onDataWritten(String(data.getData(), data.getSize()));
}

};
