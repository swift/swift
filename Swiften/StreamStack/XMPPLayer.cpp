/*
 * Copyright (c) 2010 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#include "Swiften/StreamStack/XMPPLayer.h"
#include "Swiften/Parser/XMPPParser.h"
#include "Swiften/Serializer/XMPPSerializer.h"
#include "Swiften/Elements/ProtocolHeader.h"

namespace Swift {

XMPPLayer::XMPPLayer(
		PayloadParserFactoryCollection* payloadParserFactories,
		PayloadSerializerCollection* payloadSerializers) : 
			payloadParserFactories_(payloadParserFactories), 
			payloadSerializers_(payloadSerializers),
			resetParserAfterParse_(false),
			inParser_(false) {
	xmppParser_ = new XMPPParser(this, payloadParserFactories_);
	xmppSerializer_ = new XMPPSerializer(payloadSerializers_);
}

XMPPLayer::~XMPPLayer() {
	delete xmppSerializer_;
	delete xmppParser_;
}

void XMPPLayer::writeHeader(const ProtocolHeader& header) {
	onWriteData(ByteArray(xmppSerializer_->serializeHeader(header)));
}

void XMPPLayer::writeFooter() {
	onWriteData(ByteArray(xmppSerializer_->serializeFooter()));
}

void XMPPLayer::writeElement(boost::shared_ptr<Element> element) {
	onWriteData(ByteArray(xmppSerializer_->serializeElement(element)));
}

void XMPPLayer::writeData(const String& data) {
	onWriteData(ByteArray(data));
}

void XMPPLayer::parseData(ByteArray data) {
	onDataRead(data);
	inParser_ = true;
	if (!xmppParser_->parse(String(data.getData(), data.getSize()))) {
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
	xmppParser_ = new XMPPParser(this, payloadParserFactories_);
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
