/*
 * Copyright (c) 2010-2018 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#include <Swiften/Session/BasicSessionStream.h>

#include <memory>

#include <boost/bind.hpp>

#include <Swiften/StreamStack/CompressionLayer.h>
#include <Swiften/StreamStack/ConnectionLayer.h>
#include <Swiften/StreamStack/StreamStack.h>
#include <Swiften/StreamStack/TLSLayer.h>
#include <Swiften/StreamStack/WhitespacePingLayer.h>
#include <Swiften/StreamStack/XMPPLayer.h>
#include <Swiften/TLS/TLSContext.h>
#include <Swiften/TLS/TLSContextFactory.h>

namespace Swift {

BasicSessionStream::BasicSessionStream(
        StreamType streamType,
        std::shared_ptr<Connection> connection,
        PayloadParserFactoryCollection* payloadParserFactories,
        PayloadSerializerCollection* payloadSerializers,
        TLSContextFactory* tlsContextFactory,
        TimerFactory* timerFactory,
        XMLParserFactory* xmlParserFactory,
        const TLSOptions& tlsOptions) :
            available(false),
            connection(connection),
            tlsContextFactory(tlsContextFactory),
            timerFactory(timerFactory),
            tlsOptions_(tlsOptions) {
    auto xmppLayer = std::make_unique<XMPPLayer>(payloadParserFactories, payloadSerializers, xmlParserFactory, streamType);
    xmppLayer->onStreamStart.connect(boost::bind(&BasicSessionStream::handleStreamStartReceived, this, _1));
    xmppLayer->onStreamEnd.connect(boost::bind(&BasicSessionStream::handleStreamEndReceived, this));
    xmppLayer->onElement.connect(boost::bind(&BasicSessionStream::handleElementReceived, this, _1));
    xmppLayer->onError.connect(boost::bind(&BasicSessionStream::handleXMPPError, this));
    xmppLayer->onDataRead.connect(boost::bind(&BasicSessionStream::handleDataRead, this, _1));
    xmppLayer->onWriteData.connect(boost::bind(&BasicSessionStream::handleDataWritten, this, _1));

    connection->onDisconnected.connect(boost::bind(&BasicSessionStream::handleConnectionFinished, this, _1));

    streamStack = std::make_unique<StreamStack>(std::move(xmppLayer), std::unique_ptr<ConnectionLayer>(new ConnectionLayer(connection)));
    available = true;
}

BasicSessionStream::~BasicSessionStream() {

    if (auto tlsLayer = streamStack->getLayer<TLSLayer>()) {
        tlsLayer->onError.disconnect(boost::bind(&BasicSessionStream::handleTLSError, this, _1));
        tlsLayer->onConnected.disconnect(boost::bind(&BasicSessionStream::handleTLSConnected, this));
    }

    connection->onDisconnected.disconnect(boost::bind(&BasicSessionStream::handleConnectionFinished, this, _1));

    auto xmppLayer = streamStack->getLayer<XMPPLayer>();
    xmppLayer->onStreamStart.disconnect(boost::bind(&BasicSessionStream::handleStreamStartReceived, this, _1));
    xmppLayer->onStreamEnd.disconnect(boost::bind(&BasicSessionStream::handleStreamEndReceived, this));
    xmppLayer->onElement.disconnect(boost::bind(&BasicSessionStream::handleElementReceived, this, _1));
    xmppLayer->onError.disconnect(boost::bind(&BasicSessionStream::handleXMPPError, this));
    xmppLayer->onDataRead.disconnect(boost::bind(&BasicSessionStream::handleDataRead, this, _1));
    xmppLayer->onWriteData.disconnect(boost::bind(&BasicSessionStream::handleDataWritten, this, _1));
}

void BasicSessionStream::writeHeader(const ProtocolHeader& header) {
    assert(available);
    auto* xmppLayer = streamStack->getLayer<XMPPLayer>();
    xmppLayer->writeHeader(header);
}

void BasicSessionStream::writeElement(std::shared_ptr<ToplevelElement> element) {
    assert(available);
    auto* xmppLayer = streamStack->getLayer<XMPPLayer>();
    xmppLayer->writeElement(element);
}

void BasicSessionStream::writeFooter() {
    assert(available);
    auto* xmppLayer = streamStack->getLayer<XMPPLayer>();
    xmppLayer->writeFooter();
}

void BasicSessionStream::writeData(const std::string& data) {
    assert(available);
    auto* xmppLayer = streamStack->getLayer<XMPPLayer>();
    xmppLayer->writeData(data);
}

void BasicSessionStream::close() {
    connection->disconnect();
}

bool BasicSessionStream::isOpen() {
    return available;
}

bool BasicSessionStream::supportsTLSEncryption() {
    return tlsContextFactory && tlsContextFactory->canCreate();
}

void BasicSessionStream::addTLSEncryption() {
    assert(available);
    auto tlsContext = tlsContextFactory->createTLSContext(tlsOptions_);
    auto tlsLayer = std::make_unique<TLSLayer>(std::move(tlsContext));
    if (hasTLSCertificate() && !tlsLayer->setClientCertificate(getTLSCertificate())) {
        onClosed(std::make_shared<SessionStreamError>(SessionStreamError::InvalidTLSCertificateError));
    }
    else {
        streamStack->addLayer(std::move(tlsLayer));
        auto tlsLayer = streamStack->getLayer<TLSLayer>();
        tlsLayer->onError.connect(boost::bind(&BasicSessionStream::handleTLSError, this, _1));
        tlsLayer->onConnected.connect(boost::bind(&BasicSessionStream::handleTLSConnected, this));
        tlsLayer->connect();
    }
}

bool BasicSessionStream::isTLSEncrypted() {
    return streamStack->getLayer<TLSLayer>();
}

Certificate::ref BasicSessionStream::getPeerCertificate() const {
    return streamStack->getLayer<TLSLayer>()->getPeerCertificate();
}

std::vector<Certificate::ref> BasicSessionStream::getPeerCertificateChain() const {
    return streamStack->getLayer<TLSLayer>()->getPeerCertificateChain();
}

std::shared_ptr<CertificateVerificationError> BasicSessionStream::getPeerCertificateVerificationError() const {
    return streamStack->getLayer<TLSLayer>()->getPeerCertificateVerificationError();
}

ByteArray BasicSessionStream::getTLSFinishMessage() const {
    return streamStack->getLayer<TLSLayer>()->getContext()->getFinishMessage();
}

bool BasicSessionStream::supportsZLibCompression() {
    return true;
}

void BasicSessionStream::addZLibCompression() {
    streamStack->addLayer(std::make_unique<CompressionLayer>());
}

void BasicSessionStream::setWhitespacePingEnabled(bool enabled) {
    auto whitespacePingLayer = streamStack->getLayer<WhitespacePingLayer>();
    if (enabled) {
        if (!whitespacePingLayer) {
            streamStack->addLayer(std::make_unique<WhitespacePingLayer>(timerFactory));
            whitespacePingLayer = streamStack->getLayer<WhitespacePingLayer>();
        }
        whitespacePingLayer->setActive();
    }
    else if (whitespacePingLayer) {
        whitespacePingLayer->setInactive();
    }
}

void BasicSessionStream::resetXMPPParser() {
    auto* xmppLayer = streamStack->getLayer<XMPPLayer>();
    xmppLayer->resetParser();
}

void BasicSessionStream::handleStreamStartReceived(const ProtocolHeader& header) {
    onStreamStartReceived(header);
}

void BasicSessionStream::handleStreamEndReceived() {
    onStreamEndReceived();
}

void BasicSessionStream::handleElementReceived(std::shared_ptr<ToplevelElement> element) {
    onElementReceived(element);
}

void BasicSessionStream::handleXMPPError() {
    available = false;
    onClosed(std::make_shared<SessionStreamError>(SessionStreamError::ParseError));
}

void BasicSessionStream::handleTLSConnected() {
    onTLSEncrypted();
}

void BasicSessionStream::handleTLSError(std::shared_ptr<TLSError> error) {
    available = false;
    onClosed(error);
}

void BasicSessionStream::handleConnectionFinished(const boost::optional<Connection::Error>& error) {
    available = false;
    if (error == Connection::ReadError) {
        onClosed(std::make_shared<SessionStreamError>(SessionStreamError::ConnectionReadError));
    }
    else if (error) {
        onClosed(std::make_shared<SessionStreamError>(SessionStreamError::ConnectionWriteError));
    }
    else {
        onClosed(std::shared_ptr<SessionStreamError>());
    }
}

void BasicSessionStream::handleDataRead(const SafeByteArray& data) {
    onDataRead(data);
}

void BasicSessionStream::handleDataWritten(const SafeByteArray& data) {
    onDataWritten(data);
}

}
