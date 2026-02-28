/*
 * Copyright (c) 2010-2016 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#include <Swiften/StreamStack/XMPPLayer.h>

#include <Swiften/Elements/ProtocolHeader.h>
#include <Swiften/Parser/XMPPParser.h>
#include <Swiften/Serializer/XMPPSerializer.h>

namespace Swift {

XMPPLayer::XMPPLayer(
        PayloadParserFactoryCollection* payloadParserFactories,
        PayloadSerializerCollection* payloadSerializers,
        XMLParserFactory* xmlParserFactory,
        StreamType streamType,
        bool setExplictNSonTopLevelElements) :
            payloadParserFactories_(payloadParserFactories),
            payloadSerializers_(payloadSerializers),
            xmlParserFactory_(xmlParserFactory),
            setExplictNSonTopLevelElements_(setExplictNSonTopLevelElements),
            resetParserAfterParse_(false),
            inParser_(false) {
    xmppParser_ = new XMPPParser(this, payloadParserFactories_, xmlParserFactory);
    xmppSerializer_ = new XMPPSerializer(payloadSerializers_, streamType, setExplictNSonTopLevelElements);
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

void XMPPLayer::writeElement(std::shared_ptr<ToplevelElement> element) {
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

void XMPPLayer::handleElement(std::shared_ptr<ToplevelElement> stanza) {
    onElement(stanza);
}

void XMPPLayer::handleStreamEnd() {
    onStreamEnd();
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
