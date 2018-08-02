/*
 * Copyright (c) 2010-2018 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#include <Swiften/Session/Session.h>

#include <boost/bind.hpp>

#include <Swiften/StreamStack/StreamStack.h>
#include <Swiften/StreamStack/XMPPLayer.h>

namespace Swift {

Session::Session(
        std::shared_ptr<Connection> connection,
        PayloadParserFactoryCollection* payloadParserFactories,
        PayloadSerializerCollection* payloadSerializers,
        XMLParserFactory* xmlParserFactory) :
            connection(connection),
            payloadParserFactories(payloadParserFactories),
            payloadSerializers(payloadSerializers),
            xmlParserFactory(xmlParserFactory),
            finishing(false) {
}

Session::~Session() {
}

void Session::startSession() {
    initializeStreamStack();
    handleSessionStarted();
}

void Session::finishSession() {
    if (finishing) {
        return;
    }
    finishing = true;
    if (auto xmppLayer = getXMPPLayer()) {
        xmppLayer->writeFooter();
    }
    connection->disconnect();
}

void Session::finishSession(const SessionError& /*error*/) {
    if (finishing) {
        return;
    }
    finishing = true;
    if (auto xmppLayer = getXMPPLayer()) {
        xmppLayer->writeFooter();
    }
    connection->disconnect();
}

void Session::initializeStreamStack() {
    auto xmppLayer = std::unique_ptr<XMPPLayer>(new XMPPLayer(payloadParserFactories, payloadSerializers, xmlParserFactory, ClientStreamType));
    xmppLayer->onStreamStart.connect(
            boost::bind(&Session::handleStreamStart, this, _1));
    xmppLayer->onElement.connect(boost::bind(&Session::handleElement, this, _1));
    xmppLayer->onError.connect(
            boost::bind(&Session::finishSession, this, XMLError));
    xmppLayer->onDataRead.connect(boost::bind(boost::ref(onDataRead), _1));
    xmppLayer->onWriteData.connect(boost::bind(boost::ref(onDataWritten), _1));
    connection->onDisconnected.connect(
            boost::bind(&Session::handleDisconnected, this, _1));
    streamStack = std::unique_ptr<StreamStack>(new StreamStack(std::move(xmppLayer), std::unique_ptr<ConnectionLayer>(new ConnectionLayer(connection))));
}

XMPPLayer* Session::getXMPPLayer() const {
    return dynamic_cast<XMPPLayer*>(streamStack->getTopLayer());
}

StreamStack* Session::getStreamStack() const {
    return streamStack.get();
}


void Session::sendElement(std::shared_ptr<ToplevelElement> stanza) {
    getXMPPLayer()->writeElement(stanza);
}

void Session::handleDisconnected(const boost::optional<Connection::Error>& connectionError) {
    connection->onDisconnected.disconnect(
            boost::bind(&Session::handleDisconnected, this, _1));
    if (connectionError) {
        switch (*connectionError) {
            case Connection::ReadError:
                handleSessionFinished(ConnectionReadError);
                onSessionFinished(ConnectionReadError);
                break;
            case Connection::WriteError:
                handleSessionFinished(ConnectionWriteError);
                onSessionFinished(ConnectionWriteError);
                break;
        }
    }
    else {
        boost::optional<SessionError> error;
        handleSessionFinished(error);
        onSessionFinished(error);
    }
}

}
