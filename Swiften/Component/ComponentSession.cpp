/*
 * Copyright (c) 2010-2016 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#include <Swiften/Component/ComponentSession.h>

#include <memory>

#include <boost/bind.hpp>

#include <Swiften/Component/ComponentHandshakeGenerator.h>
#include <Swiften/Elements/ComponentHandshake.h>
#include <Swiften/Elements/ProtocolHeader.h>
#include <Swiften/Elements/StreamFeatures.h>
#include <Swiften/Session/SessionStream.h>

namespace Swift {

ComponentSession::ComponentSession(const JID& jid, const std::string& secret, std::shared_ptr<SessionStream> stream, CryptoProvider* crypto) : jid(jid), secret(secret), stream(stream), crypto(crypto), state(Initial) {
}

ComponentSession::~ComponentSession() {
}

void ComponentSession::start() {
    stream->onStreamStartReceived.connect(boost::bind(&ComponentSession::handleStreamStart, shared_from_this(), _1));
    stream->onElementReceived.connect(boost::bind(&ComponentSession::handleElement, shared_from_this(), _1));
    stream->onClosed.connect(boost::bind(&ComponentSession::handleStreamClosed, shared_from_this(), _1));

    assert(state == Initial);
    state = WaitingForStreamStart;
    sendStreamHeader();
}

void ComponentSession::sendStreamHeader() {
    ProtocolHeader header;
    header.setTo(jid);
    stream->writeHeader(header);
}

void ComponentSession::sendStanza(std::shared_ptr<Stanza> stanza) {
    stream->writeElement(stanza);
}

void ComponentSession::handleStreamStart(const ProtocolHeader& header) {
    checkState(WaitingForStreamStart);
    state = Authenticating;
    stream->writeElement(ComponentHandshake::ref(new ComponentHandshake(ComponentHandshakeGenerator::getHandshake(header.getID(), secret, crypto))));
}

void ComponentSession::handleElement(std::shared_ptr<ToplevelElement> element) {
    if (std::shared_ptr<Stanza> stanza = std::dynamic_pointer_cast<Stanza>(element)) {
        if (getState() == Initialized) {
            onStanzaReceived(stanza);
        }
        else {
            finishSession(Error::UnexpectedElementError);
        }
    }
    else if (std::dynamic_pointer_cast<ComponentHandshake>(element)) {
        if (!checkState(Authenticating)) {
            return;
        }
        stream->setWhitespacePingEnabled(true);
        state = Initialized;
        onInitialized();
    }
    else if (getState() == Authenticating) {
        if (std::dynamic_pointer_cast<StreamFeatures>(element)) {
            // M-Link sends stream features, so swallow that.
        }
        else {
            // FIXME: We should actually check the element received
            finishSession(Error::AuthenticationFailedError);
        }
    }
    else {
        finishSession(Error::UnexpectedElementError);
    }
}

bool ComponentSession::checkState(State state) {
    if (this->state != state) {
        finishSession(Error::UnexpectedElementError);
        return false;
    }
    return true;
}

void ComponentSession::handleStreamClosed(std::shared_ptr<Swift::Error> streamError) {
    State oldState = state;
    state = Finished;
    stream->setWhitespacePingEnabled(false);
    stream->onStreamStartReceived.disconnect(boost::bind(&ComponentSession::handleStreamStart, shared_from_this(), _1));
    stream->onElementReceived.disconnect(boost::bind(&ComponentSession::handleElement, shared_from_this(), _1));
    stream->onClosed.disconnect(boost::bind(&ComponentSession::handleStreamClosed, shared_from_this(), _1));
    if (oldState == Finishing) {
        onFinished(error);
    }
    else {
        onFinished(streamError);
    }
}

void ComponentSession::finish() {
    finishSession(std::shared_ptr<Error>());
}

void ComponentSession::finishSession(Error::Type error) {
    finishSession(std::make_shared<Swift::ComponentSession::Error>(error));
}

void ComponentSession::finishSession(std::shared_ptr<Swift::Error> finishError) {
    state = Finishing;
    error = finishError;
    assert(stream->isOpen());
    stream->writeFooter();
    stream->close();
}

}
