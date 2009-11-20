#include "Swiften/Client/ClientSession.h"

#include <boost/bind.hpp>

#include "Swiften/Elements/ProtocolHeader.h"
#include "Swiften/Elements/StreamFeatures.h"
#include "Swiften/Elements/StartTLSRequest.h"
#include "Swiften/Elements/StartTLSFailure.h"
#include "Swiften/Elements/TLSProceed.h"
#include "Swiften/Elements/AuthRequest.h"
#include "Swiften/Elements/AuthSuccess.h"
#include "Swiften/Elements/AuthFailure.h"
#include "Swiften/Elements/AuthChallenge.h"
#include "Swiften/Elements/AuthResponse.h"
#include "Swiften/Elements/StartSession.h"
#include "Swiften/Elements/IQ.h"
#include "Swiften/Elements/ResourceBind.h"
#include "Swiften/SASL/PLAINClientAuthenticator.h"
#include "Swiften/SASL/SCRAMSHA1ClientAuthenticator.h"
#include "Swiften/Session/SessionStream.h"

namespace Swift {

ClientSession::ClientSession(
		const JID& jid, 
		boost::shared_ptr<SessionStream> stream) :
			localJID(jid),	
			state(Initial), 
			stream(stream),
			needSessionStart(false),
			authenticator(NULL) {
}

void ClientSession::start() {
	stream->onStreamStartReceived.connect(boost::bind(&ClientSession::handleStreamStart, shared_from_this(), _1));
	stream->onElementReceived.connect(boost::bind(&ClientSession::handleElement, shared_from_this(), _1));
	stream->onError.connect(boost::bind(&ClientSession::handleStreamError, shared_from_this(), _1));
	stream->onTLSEncrypted.connect(boost::bind(&ClientSession::handleTLSEncrypted, shared_from_this()));

	assert(state == Initial);
	state = WaitingForStreamStart;
	sendStreamHeader();
}

void ClientSession::sendStreamHeader() {
	ProtocolHeader header;
	header.setTo(getRemoteJID());
	stream->writeHeader(header);
}

void ClientSession::sendElement(boost::shared_ptr<Element> element) {
	stream->writeElement(element);
}

void ClientSession::handleStreamStart(const ProtocolHeader&) {
	checkState(WaitingForStreamStart);
	state = Negotiating;
}

void ClientSession::handleElement(boost::shared_ptr<Element> element) {
	if (getState() == Initialized) {
		onElementReceived(element);
	}
	else if (StreamFeatures* streamFeatures = dynamic_cast<StreamFeatures*>(element.get())) {
		if (!checkState(Negotiating)) {
			return;
		}

		if (streamFeatures->hasStartTLS() && stream->supportsTLSEncryption()) {
			state = WaitingForEncrypt;
			stream->writeElement(boost::shared_ptr<StartTLSRequest>(new StartTLSRequest()));
		}
		else if (streamFeatures->hasAuthenticationMechanisms()) {
			if (stream->hasTLSCertificate()) {
				if (streamFeatures->hasAuthenticationMechanism("EXTERNAL")) {
					state = Authenticating;
					stream->writeElement(boost::shared_ptr<Element>(new AuthRequest("EXTERNAL", "")));
				}
				else {
					finishSession(Error::TLSClientCertificateError);
				}
			}
			/*else if (streamFeatures->hasAuthenticationMechanism("SCRAM-SHA-1")) {
				// FIXME: Use a real nonce
				authenticator = new SCRAMSHA1ClientAuthenticator(ByteArray("\x01\x02\x03\x04\x05\x06\x07\x08", 8));
				state = WaitingForCredentials;
				onNeedCredentials();
			}*/
			else if (streamFeatures->hasAuthenticationMechanism("PLAIN")) {
				authenticator = new PLAINClientAuthenticator();
				state = WaitingForCredentials;
				onNeedCredentials();
			}
			else {
				finishSession(Error::NoSupportedAuthMechanismsError);
			}
		}
		else {
			// Start the session
			stream->setWhitespacePingEnabled(true);

			if (streamFeatures->hasSession()) {
				needSessionStart = true;
			}

			if (streamFeatures->hasResourceBind()) {
				state = BindingResource;
				boost::shared_ptr<ResourceBind> resourceBind(new ResourceBind());
				if (!localJID.getResource().isEmpty()) {
					resourceBind->setResource(localJID.getResource());
				}
				stream->writeElement(IQ::createRequest(IQ::Set, JID(), "session-bind", resourceBind));
			}
			else if (needSessionStart) {
				sendSessionStart();
			}
			else {
				state = Initialized;
				onInitialized();
			}
		}
	}
	else if (AuthChallenge* challenge = dynamic_cast<AuthChallenge*>(element.get())) {
		checkState(Authenticating);
		assert(authenticator);
		if (authenticator->setChallenge(challenge->getValue())) {
			stream->writeElement(boost::shared_ptr<AuthResponse>(new AuthResponse(authenticator->getResponse())));
		}
		else {
			finishSession(Error::AuthenticationFailedError);
		}
	}
	else if (dynamic_cast<AuthSuccess*>(element.get())) {
		checkState(Authenticating);
		state = WaitingForStreamStart;
		delete authenticator;
		authenticator = NULL;
		stream->resetXMPPParser();
		sendStreamHeader();
	}
	else if (dynamic_cast<AuthFailure*>(element.get())) {
		delete authenticator;
		authenticator = NULL;
		finishSession(Error::AuthenticationFailedError);
	}
	else if (dynamic_cast<TLSProceed*>(element.get())) {
		checkState(WaitingForEncrypt);
		state = Encrypting;
		stream->addTLSEncryption();
	}
	else if (dynamic_cast<StartTLSFailure*>(element.get())) {
		finishSession(Error::TLSError);
	}
	else if (IQ* iq = dynamic_cast<IQ*>(element.get())) {
		if (state == BindingResource) {
			boost::shared_ptr<ResourceBind> resourceBind(iq->getPayload<ResourceBind>());
			if (iq->getType() == IQ::Error && iq->getID() == "session-bind") {
				finishSession(Error::ResourceBindError);
			}
			else if (!resourceBind) {
				finishSession(Error::UnexpectedElementError);
			}
			else if (iq->getType() == IQ::Result) {
				localJID = resourceBind->getJID();
				if (!localJID.isValid()) {
					finishSession(Error::ResourceBindError);
				}
				if (needSessionStart) {
					sendSessionStart();
				}
				else {
					state = Initialized;
				}
			}
			else {
				finishSession(Error::UnexpectedElementError);
			}
		}
		else if (state == StartingSession) {
			if (iq->getType() == IQ::Result) {
				state = Initialized;
				onInitialized();
			}
			else if (iq->getType() == IQ::Error) {
				finishSession(Error::SessionStartError);
			}
			else {
				finishSession(Error::UnexpectedElementError);
			}
		}
		else {
			finishSession(Error::UnexpectedElementError);
		}
	}
	else {
		// FIXME Not correct?
		state = Initialized;
		onInitialized();
	}
}

void ClientSession::sendSessionStart() {
	state = StartingSession;
	stream->writeElement(IQ::createRequest(IQ::Set, JID(), "session-start", boost::shared_ptr<StartSession>(new StartSession())));
}

bool ClientSession::checkState(State state) {
	if (state != state) {
		finishSession(Error::UnexpectedElementError);
		return false;
	}
	return true;
}

void ClientSession::sendCredentials(const String& password) {
	assert(WaitingForCredentials);
	state = Authenticating;
	authenticator->setCredentials(localJID.getNode(), password);
	stream->writeElement(boost::shared_ptr<AuthRequest>(new AuthRequest(authenticator->getName(), authenticator->getResponse())));
}

void ClientSession::handleTLSEncrypted() {
	checkState(WaitingForEncrypt);
	state = WaitingForStreamStart;
	stream->resetXMPPParser();
	sendStreamHeader();
}

void ClientSession::handleStreamError(boost::shared_ptr<Swift::Error> error) {
	finishSession(error);
}

void ClientSession::finish() {
	if (stream->isAvailable()) {
		stream->writeFooter();
	}
	finishSession(boost::shared_ptr<Error>());
}

void ClientSession::finishSession(Error::Type error) {
	finishSession(boost::shared_ptr<Swift::ClientSession::Error>(new Swift::ClientSession::Error(error)));
}

void ClientSession::finishSession(boost::shared_ptr<Swift::Error> error) {
	state = Finished;
	stream->setWhitespacePingEnabled(false);
	onFinished(error);
}


}
