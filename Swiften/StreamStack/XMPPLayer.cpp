#include "Swiften/StreamStack/XMPPLayer.h"
#include "Swiften/Parser/XMPPParser.h"
#include "Swiften/Serializer/XMPPSerializer.h"

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

void XMPPLayer::writeHeader(const String& domain) {
	onWriteData(ByteArray(xmppSerializer_->serializeHeader(domain)));
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

void XMPPLayer::handleStreamStart(const String& domain) {
	onStreamStart(domain);
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
