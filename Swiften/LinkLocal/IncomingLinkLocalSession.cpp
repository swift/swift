/*
 * Copyright (c) 2010-2016 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#include <Swiften/LinkLocal/IncomingLinkLocalSession.h>

#include <memory>

#include <boost/bind.hpp>

#include <Swiften/Elements/IQ.h>
#include <Swiften/Elements/ProtocolHeader.h>
#include <Swiften/Elements/StreamFeatures.h>
#include <Swiften/Network/Connection.h>
#include <Swiften/StreamStack/ConnectionLayer.h>
#include <Swiften/StreamStack/StreamStack.h>
#include <Swiften/StreamStack/XMPPLayer.h>

namespace Swift {

IncomingLinkLocalSession::IncomingLinkLocalSession(
        const JID& localJID,
        std::shared_ptr<Connection> connection,
        PayloadParserFactoryCollection* payloadParserFactories,
        PayloadSerializerCollection* payloadSerializers,
        XMLParserFactory* xmlParserFactory) :
            Session(connection, payloadParserFactories, payloadSerializers, xmlParserFactory),
            initialized(false) {
    setLocalJID(localJID);
}

void IncomingLinkLocalSession::handleStreamStart(const ProtocolHeader& incomingHeader) {
    setRemoteJID(JID(incomingHeader.getFrom()));
    if (!getRemoteJID().isValid()) {
        finishSession();
        return;
    }

    ProtocolHeader header;
    header.setFrom(getLocalJID());
    getXMPPLayer()->writeHeader(header);

    if (incomingHeader.getVersion() == "1.0") {
        getXMPPLayer()->writeElement(std::make_shared<StreamFeatures>());
    }
    else {
        setInitialized();
    }
}

void IncomingLinkLocalSession::handleElement(std::shared_ptr<ToplevelElement> element) {
    std::shared_ptr<Stanza> stanza = std::dynamic_pointer_cast<Stanza>(element);
    // If we get our first stanza before streamfeatures, our session is implicitly
    // initialized
    if (stanza && !isInitialized()) {
        setInitialized();
    }

    onElementReceived(element);
}

void IncomingLinkLocalSession::setInitialized() {
    initialized = true;
    onSessionStarted();
}



}
