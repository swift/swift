/*
 * Copyright (c) 2011 Kevin Smith
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */


#include <Swiften/Session/BOSHSessionStream.h>

#include <boost/bind.hpp>
#include <boost/random/mersenne_twister.hpp>
#include <boost/random/uniform_int.hpp>
#include <boost/random/variate_generator.hpp>

#include <Swiften/Base/foreach.h>
#include <Swiften/Elements/StreamType.h>
#include <Swiften/StreamStack/XMPPLayer.h>
#include <Swiften/StreamStack/StreamStack.h>
#include <Swiften/StreamStack/ConnectionLayer.h>
#include <Swiften/StreamStack/WhitespacePingLayer.h>
#include <Swiften/StreamStack/CompressionLayer.h>
#include <Swiften/StreamStack/TLSLayer.h>
#include <Swiften/TLS/TLSContextFactory.h>
#include <Swiften/TLS/TLSContext.h>
#include <Swiften/EventLoop/EventLoop.h>

namespace Swift {

BOSHSessionStream::BOSHSessionStream(
		const URL& boshURL,
		PayloadParserFactoryCollection* payloadParserFactories, 
		PayloadSerializerCollection* payloadSerializers, 
		ConnectionFactory* connectionFactory,
		TLSContextFactory* tlsContextFactory, 
		TimerFactory* timerFactory,
		XMLParserFactory* xmlParserFactory,
		EventLoop* eventLoop,
		DomainNameResolver* resolver,
		const std::string& to,
		const URL& boshHTTPConnectProxyURL,
		const SafeString& boshHTTPConnectProxyAuthID,
		const SafeString& boshHTTPConnectProxyAuthPassword) :
			available(false), 
			payloadParserFactories(payloadParserFactories), 
			payloadSerializers(payloadSerializers), 
			tlsContextFactory(tlsContextFactory), 
			timerFactory(timerFactory),
			xmlParserFactory(xmlParserFactory),
			eventLoop(eventLoop),
			firstHeader(true) {

	boost::mt19937 random;
	boost::uniform_int<unsigned long long> dist(0, (1LL<<53) - 1);
	random.seed(time(NULL));
	unsigned long long initialRID = boost::variate_generator<boost::mt19937&, boost::uniform_int<unsigned long long> >(random, dist)();

	connectionPool = new BOSHConnectionPool(boshURL, resolver, connectionFactory, xmlParserFactory, tlsContextFactory, timerFactory, eventLoop, to, initialRID, boshHTTPConnectProxyURL, boshHTTPConnectProxyAuthID, boshHTTPConnectProxyAuthPassword);
	connectionPool->onSessionTerminated.connect(boost::bind(&BOSHSessionStream::handlePoolSessionTerminated, this, _1));
	connectionPool->onSessionStarted.connect(boost::bind(&BOSHSessionStream::handlePoolSessionStarted, this));
	connectionPool->onXMPPDataRead.connect(boost::bind(&BOSHSessionStream::handlePoolXMPPDataRead, this, _1));
	connectionPool->onBOSHDataRead.connect(boost::bind(&BOSHSessionStream::handlePoolBOSHDataRead, this, _1));
	connectionPool->onBOSHDataWritten.connect(boost::bind(&BOSHSessionStream::handlePoolBOSHDataWritten, this, _1));

	xmppLayer = new XMPPLayer(payloadParserFactories, payloadSerializers, xmlParserFactory, ClientStreamType);
	xmppLayer->onStreamStart.connect(boost::bind(&BOSHSessionStream::handleStreamStartReceived, this, _1));
	xmppLayer->onElement.connect(boost::bind(&BOSHSessionStream::handleElementReceived, this, _1));
	xmppLayer->onError.connect(boost::bind(&BOSHSessionStream::handleXMPPError, this));
	xmppLayer->onWriteData.connect(boost::bind(&BOSHSessionStream::handleXMPPLayerDataWritten, this, _1));

	available = true;
}

BOSHSessionStream::~BOSHSessionStream() {
	close();
	connectionPool->onSessionTerminated.disconnect(boost::bind(&BOSHSessionStream::handlePoolSessionTerminated, this, _1));
	connectionPool->onSessionStarted.disconnect(boost::bind(&BOSHSessionStream::handlePoolSessionStarted, this));
	connectionPool->onXMPPDataRead.disconnect(boost::bind(&BOSHSessionStream::handlePoolXMPPDataRead, this, _1));
	connectionPool->onBOSHDataRead.disconnect(boost::bind(&BOSHSessionStream::handlePoolBOSHDataRead, this, _1));
	connectionPool->onBOSHDataWritten.disconnect(boost::bind(&BOSHSessionStream::handlePoolBOSHDataWritten, this, _1));
	delete connectionPool;
	connectionPool = NULL;
	xmppLayer->onStreamStart.disconnect(boost::bind(&BOSHSessionStream::handleStreamStartReceived, this, _1));
	xmppLayer->onElement.disconnect(boost::bind(&BOSHSessionStream::handleElementReceived, this, _1));
	xmppLayer->onError.disconnect(boost::bind(&BOSHSessionStream::handleXMPPError, this));
	xmppLayer->onWriteData.disconnect(boost::bind(&BOSHSessionStream::handleXMPPLayerDataWritten, this, _1));
	delete xmppLayer;
	xmppLayer = NULL;
}

void BOSHSessionStream::handlePoolXMPPDataRead(const SafeByteArray& data) {
	xmppLayer->handleDataRead(data);
}

void BOSHSessionStream::writeElement(boost::shared_ptr<Element> element) {
	assert(available);
	xmppLayer->writeElement(element);
}

void BOSHSessionStream::writeFooter() {
	connectionPool->writeFooter();
}

void BOSHSessionStream::writeData(const std::string& data) {
	assert(available);
	xmppLayer->writeData(data);
}

void BOSHSessionStream::close() {
	connectionPool->close();
}

bool BOSHSessionStream::isOpen() {
	return available;
}

bool BOSHSessionStream::supportsTLSEncryption() {
	return false;
}

void BOSHSessionStream::addTLSEncryption() {
	assert(available);
}

bool BOSHSessionStream::isTLSEncrypted() {
	return false;
}

Certificate::ref BOSHSessionStream::getPeerCertificate() const {
	return Certificate::ref();
}

boost::shared_ptr<CertificateVerificationError> BOSHSessionStream::getPeerCertificateVerificationError() const {
	return boost::shared_ptr<CertificateVerificationError>();
}

ByteArray BOSHSessionStream::getTLSFinishMessage() const {
	return ByteArray();
}

bool BOSHSessionStream::supportsZLibCompression() {
	return false;
}

void BOSHSessionStream::addZLibCompression() {

}

void BOSHSessionStream::setWhitespacePingEnabled(bool /*enabled*/) {
	return;
}

void BOSHSessionStream::resetXMPPParser() {
	xmppLayer->resetParser();
}

void BOSHSessionStream::handleStreamStartReceived(const ProtocolHeader& header) {
	onStreamStartReceived(header);
}

void BOSHSessionStream::handleElementReceived(boost::shared_ptr<Element> element) {
	onElementReceived(element);
}

void BOSHSessionStream::handleXMPPError() {
	available = false;
	onClosed(boost::make_shared<SessionStreamError>(SessionStreamError::ParseError));
}

void BOSHSessionStream::handlePoolSessionStarted() {
	fakeStreamHeaderReceipt();
}

void BOSHSessionStream::handlePoolSessionTerminated(BOSHError::ref error) {
	eventLoop->postEvent(boost::bind(&BOSHSessionStream::fakeStreamFooterReceipt, this, error), shared_from_this());
}

void BOSHSessionStream::writeHeader(const ProtocolHeader& header) {
	streamHeader = header;
	/*First time we're told to do this, don't (the sending of the initial header is handled on connect)
	  On subsequent requests we should restart the stream the BOSH way.
	*/
	if (!firstHeader) {
		eventLoop->postEvent(boost::bind(&BOSHSessionStream::fakeStreamHeaderReceipt, this), shared_from_this());
		eventLoop->postEvent(boost::bind(&BOSHConnectionPool::restartStream, connectionPool), shared_from_this());
	}
	firstHeader = false;
}


void BOSHSessionStream::fakeStreamHeaderReceipt() {
	std::stringstream header;
	header << "<stream:stream xmlns='jabber:client' xmlns:stream='http://etherx.jabber.org/streams' from='";
	header << streamHeader.getTo() << "' id='dummy' version='1.0'>";

	xmppLayer->handleDataRead(createSafeByteArray(header.str()));
}

void BOSHSessionStream::fakeStreamFooterReceipt(BOSHError::ref error) {
	std::string footer("</stream:stream>");
	xmppLayer->handleDataRead(createSafeByteArray(footer));
	onClosed(error);
}

void BOSHSessionStream::handleXMPPLayerDataWritten(const SafeByteArray& data) {
	eventLoop->postEvent(boost::bind(&BOSHConnectionPool::write, connectionPool, data), shared_from_this());
}

void BOSHSessionStream::handlePoolBOSHDataRead(const SafeByteArray& data) {
	onDataRead(data);
}

void BOSHSessionStream::handlePoolBOSHDataWritten(const SafeByteArray& data) {
	onDataWritten(data);
}

};
