#include "Swiften/Client/ClientSession.h"

#include <boost/bind.hpp>

#include "Swiften/Network/ConnectionFactory.h"
#include "Swiften/Elements/ProtocolHeader.h"
#include "Swiften/StreamStack/StreamStack.h"
#include "Swiften/StreamStack/ConnectionLayer.h"
#include "Swiften/StreamStack/XMPPLayer.h"
#include "Swiften/StreamStack/TLSLayer.h"
#include "Swiften/StreamStack/TLSLayerFactory.h"
#include "Swiften/Elements/StreamFeatures.h"
#include "Swiften/Elements/StartTLSRequest.h"
#include "Swiften/Elements/StartTLSFailure.h"
#include "Swiften/Elements/TLSProceed.h"
#include "Swiften/Elements/AuthRequest.h"
#include "Swiften/Elements/AuthSuccess.h"
#include "Swiften/Elements/AuthFailure.h"
#include "Swiften/Elements/StartSession.h"
#include "Swiften/Elements/IQ.h"
#include "Swiften/Elements/ResourceBind.h"
#include "Swiften/SASL/PLAINMessage.h"
#include "Swiften/StreamStack/WhitespacePingLayer.h"

namespace Swift {

ClientSession::ClientSession(
		const JID& jid, 
		boost::shared_ptr<Connection> connection,
		TLSLayerFactory* tlsLayerFactory, 
		PayloadParserFactoryCollection* payloadParserFactories, 
		PayloadSerializerCollection* payloadSerializers) : 
			Session(connection, payloadParserFactories, payloadSerializers),
			jid_(jid), 
			tlsLayerFactory_(tlsLayerFactory),
			state_(Initial), 
			needSessionStart_(false) {
}

void ClientSession::handleSessionStarted() {
	assert(state_ == Initial);
	state_ = WaitingForStreamStart;
	sendStreamHeader();
}

void ClientSession::sendStreamHeader() {
	ProtocolHeader header;
	header.setTo(jid_.getDomain());
	getXMPPLayer()->writeHeader(header);
}

void ClientSession::setCertificate(const PKCS12Certificate& certificate) {
	certificate_ = certificate;
}

void ClientSession::handleStreamStart(const ProtocolHeader&) {
	checkState(WaitingForStreamStart);
	state_ = Negotiating;
}

void ClientSession::handleElement(boost::shared_ptr<Element> element) {
	if (getState() == SessionStarted) {
		onElementReceived(element);
	}
	else if (StreamFeatures* streamFeatures = dynamic_cast<StreamFeatures*>(element.get())) {
		if (!checkState(Negotiating)) {
			return;
		}

		if (streamFeatures->hasStartTLS() && tlsLayerFactory_->canCreate()) {
			state_ = Encrypting;
			getXMPPLayer()->writeElement(boost::shared_ptr<StartTLSRequest>(new StartTLSRequest()));
		}
		else if (streamFeatures->hasAuthenticationMechanisms()) {
			if (!certificate_.isNull()) {
				if (streamFeatures->hasAuthenticationMechanism("EXTERNAL")) {
					state_ = Authenticating;
					getXMPPLayer()->writeElement(boost::shared_ptr<Element>(new AuthRequest("EXTERNAL", "")));
				}
				else {
					finishSession(ClientCertificateError);
				}
			}
			else if (streamFeatures->hasAuthenticationMechanism("PLAIN")) {
				state_ = WaitingForCredentials;
				onNeedCredentials();
			}
			else {
				finishSession(NoSupportedAuthMechanismsError);
			}
		}
		else {
			// Start the session

			// Add a whitespace ping layer
			whitespacePingLayer_ = boost::shared_ptr<WhitespacePingLayer>(new WhitespacePingLayer());
			getStreamStack()->addLayer(whitespacePingLayer_);

			if (streamFeatures->hasSession()) {
				needSessionStart_ = true;
			}

			if (streamFeatures->hasResourceBind()) {
				state_ = BindingResource;
				boost::shared_ptr<ResourceBind> resourceBind(new ResourceBind());
				if (!jid_.getResource().isEmpty()) {
					resourceBind->setResource(jid_.getResource());
				}
				getXMPPLayer()->writeElement(IQ::createRequest(IQ::Set, JID(), "session-bind", resourceBind));
			}
			else if (needSessionStart_) {
				sendSessionStart();
			}
			else {
				state_ = SessionStarted;
				setInitialized();
			}
		}
	}
	else if (dynamic_cast<AuthSuccess*>(element.get())) {
		checkState(Authenticating);
		state_ = WaitingForStreamStart;
		getXMPPLayer()->resetParser();
		sendStreamHeader();
	}
	else if (dynamic_cast<AuthFailure*>(element.get())) {
		finishSession(AuthenticationFailedError);
	}
	else if (dynamic_cast<TLSProceed*>(element.get())) {
		tlsLayer_ = tlsLayerFactory_->createTLSLayer();
		getStreamStack()->addLayer(tlsLayer_);
		if (!certificate_.isNull() && !tlsLayer_->setClientCertificate(certificate_)) {
			finishSession(ClientCertificateLoadError);
		}
		else {
			tlsLayer_->onConnected.connect(boost::bind(&ClientSession::handleTLSConnected, this));
			tlsLayer_->onError.connect(boost::bind(&ClientSession::handleTLSError, this));
			tlsLayer_->connect();
		}
	}
	else if (dynamic_cast<StartTLSFailure*>(element.get())) {
		finishSession(TLSError);
	}
	else if (IQ* iq = dynamic_cast<IQ*>(element.get())) {
		if (state_ == BindingResource) {
			boost::shared_ptr<ResourceBind> resourceBind(iq->getPayload<ResourceBind>());
			if (iq->getType() == IQ::Error && iq->getID() == "session-bind") {
				finishSession(ResourceBindError);
			}
			else if (!resourceBind) {
				finishSession(UnexpectedElementError);
			}
			else if (iq->getType() == IQ::Result) {
				jid_ = resourceBind->getJID();
				if (!jid_.isValid()) {
					finishSession(ResourceBindError);
				}
				if (needSessionStart_) {
					sendSessionStart();
				}
				else {
					state_ = SessionStarted;
				}
			}
			else {
				finishSession(UnexpectedElementError);
			}
		}
		else if (state_ == StartingSession) {
			if (iq->getType() == IQ::Result) {
				state_ = SessionStarted;
				setInitialized();
			}
			else if (iq->getType() == IQ::Error) {
				finishSession(SessionStartError);
			}
			else {
				finishSession(UnexpectedElementError);
			}
		}
		else {
			finishSession(UnexpectedElementError);
		}
	}
	else {
		// FIXME Not correct?
		state_ = SessionStarted;
		setInitialized();
	}
}

void ClientSession::sendSessionStart() {
	state_ = StartingSession;
	getXMPPLayer()->writeElement(IQ::createRequest(IQ::Set, JID(), "session-start", boost::shared_ptr<StartSession>(new StartSession())));
}

void ClientSession::handleSessionFinished(const boost::optional<SessionError>& error) {
	if (error) {
		assert(!error_);
		state_ = Error;
		error_ = error;
	}
	else {
		state_ = Finished;
	}
}

bool ClientSession::checkState(State state) {
	if (state_ != state) {
		finishSession(UnexpectedElementError);
		return false;
	}
	return true;
}

void ClientSession::sendCredentials(const String& password) {
	assert(WaitingForCredentials);
	state_ = Authenticating;
	getXMPPLayer()->writeElement(boost::shared_ptr<Element>(new AuthRequest("PLAIN", PLAINMessage(jid_.getNode(), password).getValue())));
}

void ClientSession::handleTLSConnected() {
	state_ = WaitingForStreamStart;
	getXMPPLayer()->resetParser();
	sendStreamHeader();
}

void ClientSession::handleTLSError() {
	finishSession(TLSError);
}

}
