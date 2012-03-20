/*
 * Copyright (c) 2010 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#include <Swiften/Component/ComponentSession.h>

#include <boost/bind.hpp>
#include <boost/smart_ptr/make_shared.hpp>

#include <Swiften/Elements/ProtocolHeader.h>
#include <Swiften/Elements/ComponentHandshake.h>
#include <Swiften/Session/SessionStream.h>
#include <Swiften/Component/ComponentHandshakeGenerator.h>

namespace Swift {

ComponentSession::ComponentSession(const JID& jid, const std::string& secret, boost::shared_ptr<SessionStream> stream) : jid(jid), secret(secret), stream(stream), state(Initial) {
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

void ComponentSession::sendStanza(boost::shared_ptr<Stanza> stanza) {
	stream->writeElement(stanza);
}

void ComponentSession::handleStreamStart(const ProtocolHeader& header) {
	checkState(WaitingForStreamStart);
	state = Authenticating;
	stream->writeElement(ComponentHandshake::ref(new ComponentHandshake(ComponentHandshakeGenerator::getHandshake(header.getID(), secret))));
}

void ComponentSession::handleElement(boost::shared_ptr<Element> element) {
	if (boost::shared_ptr<Stanza> stanza = boost::dynamic_pointer_cast<Stanza>(element)) {
		if (getState() == Initialized) {
			onStanzaReceived(stanza);
		}
		else {
			finishSession(Error::UnexpectedElementError);
		}
	}
	else if (boost::dynamic_pointer_cast<ComponentHandshake>(element)) {
		if (!checkState(Authenticating)) {
			return;
		}
		stream->setWhitespacePingEnabled(true);
		state = Initialized;
		onInitialized();
	}
	else if (getState() == Authenticating) {
		// FIXME: We should actually check the element received
		finishSession(Error::AuthenticationFailedError);
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

void ComponentSession::handleStreamClosed(boost::shared_ptr<Swift::Error> streamError) {
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
	finishSession(boost::shared_ptr<Error>());
}

void ComponentSession::finishSession(Error::Type error) {
	finishSession(boost::make_shared<Swift::ComponentSession::Error>(error));
}

void ComponentSession::finishSession(boost::shared_ptr<Swift::Error> finishError) {
	state = Finishing;
	error = finishError;
	assert(stream->isOpen());
	stream->writeFooter();
	stream->close();
}

}
