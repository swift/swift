/*
 * Copyright (c) 2010 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#include <Swiften/StreamStack/XMPPLayer.h>
#include <Swiften/Parser/XMPPParser.h>
#include <Swiften/Serializer/XMPPSerializer.h>
#include <Swiften/Elements/ProtocolHeader.h>

namespace Swift {

XMPPLayer::XMPPLayer(
		PayloadParserFactoryCollection* payloadParserFactories,
		PayloadSerializerCollection* payloadSerializers,
		XMLParserFactory* xmlParserFactory,
		StreamType streamType) :
			payloadParserFactories_(payloadParserFactories), 
			payloadSerializers_(payloadSerializers),
			xmlParserFactory_(xmlParserFactory),
			resetParserAfterParse_(false),
			inParser_(false) {
	xmppParser_ = new XMPPParser(this, payloadParserFactories_, xmlParserFactory);
	xmppSerializer_ = new XMPPSerializer(payloadSerializers_, streamType);
}

XMPPLayer::~XMPPLayer() {
	delete xmppSerializer_;
	delete xmppParser_;
}

void XMPPLayer::writeHeader(const ProtocolHeader& header) {
	writeDataInternal(createSafeByteArray(xmppSerializer_->serializeHeader(header)));
}

void XMPPLayer::writeFooter() {
	writeDataInternal(createSafeByteArray(xmppSerializer_->serializeFooter()));
}

void XMPPLayer::writeElement(boost::shared_ptr<Element> element) {
	writeDataInternal(xmppSerializer_->serializeElement(element));
}

void XMPPLayer::writeData(const std::string& data) {
	writeDataInternal(createSafeByteArray(data));
}

void XMPPLayer::writeDataInternal(const SafeByteArray& data) {
	onWriteData(data);
	writeDataToChildLayer(data);
}

void XMPPLayer::handleDataRead(const SafeByteArray& data) {
	onDataRead(data);
	inParser_ = true;
	// FIXME: Converting to unsafe string. Should be ok, since we don't take passwords
	// from the stream in clients. If servers start using this, and require safe storage,
	// we need to fix this.
	if (!xmppParser_->parse(byteArrayToString(ByteArray(data.begin(), data.end())))) {
		inParser_ = false;
		onError();
		return;
	}
	inParser_ = false;
	if (resetParserAfterParse_) {
		doResetParser();
	}
}

void XMPPLayer::doResetParser() {
	delete xmppParser_;
	xmppParser_ = new XMPPParser(this, payloadParserFactories_, xmlParserFactory_);
	resetParserAfterParse_ = false;
}

void XMPPLayer::handleStreamStart(const ProtocolHeader& header) {
	onStreamStart(header);
}

void XMPPLayer::handleElement(boost::shared_ptr<Element> stanza) {
	onElement(stanza);
}

void XMPPLayer::handleStreamEnd() {
}

void XMPPLayer::resetParser() {
	if (inParser_) {
		resetParserAfterParse_ = true;
	}
	else {
		doResetParser();
	}
}

}
