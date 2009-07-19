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
			jid_(jid), 
			tlsLayerFactory_(tlsLayerFactory),
			payloadParserFactories_(payloadParserFactories),
			payloadSerializers_(payloadSerializers),
			state_(Initial), 
			error_(NoError),
			connection_(connection),
			streamStack_(0),
			needSessionStart_(false) {
}

ClientSession::~ClientSession() {
	delete streamStack_;
}

void ClientSession::start() {
	assert(state_ == Initial);

	connection_->onDisconnected.connect(boost::bind(&ClientSession::handleDisconnected, this, _1));
	initializeStreamStack();
	state_ = WaitingForStreamStart;
	sendStreamHeader();
}

void ClientSession::stop() {
	// TODO: Send end stream header if applicable
	connection_->disconnect();
}

void ClientSession::sendStreamHeader() {
	ProtocolHeader header;
	header.setTo(jid_.getDomain());
	xmppLayer_->writeHeader(header);
}

void ClientSession::initializeStreamStack() {
	xmppLayer_ = boost::shared_ptr<XMPPLayer>(new XMPPLayer(payloadParserFactories_, payloadSerializers_));
	xmppLayer_->onStreamStart.connect(boost::bind(&ClientSession::handleStreamStart, this));
	xmppLayer_->onElement.connect(boost::bind(&ClientSession::handleElement, this, _1));
	xmppLayer_->onError.connect(boost::bind(&ClientSession::setError, this, XMLError));
	xmppLayer_->onDataRead.connect(boost::bind(boost::ref(onDataRead), _1));
	xmppLayer_->onWriteData.connect(boost::bind(boost::ref(onDataWritten), _1));
	connectionLayer_ = boost::shared_ptr<ConnectionLayer>(new ConnectionLayer(connection_));
	streamStack_ = new StreamStack(xmppLayer_, connectionLayer_);
}

void ClientSession::handleDisconnected(const boost::optional<Connection::Error>& error) {
	if (error) {
		switch (*error) {
			case Connection::ReadError:
				setError(ConnectionReadError);
				break;
			case Connection::WriteError:
				setError(ConnectionWriteError);
				break;
		}
	}
}

void ClientSession::setCertificate(const PKCS12Certificate& certificate) {
	certificate_ = certificate;
}

void ClientSession::handleStreamStart() {
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
			xmppLayer_->writeElement(boost::shared_ptr<StartTLSRequest>(new StartTLSRequest()));
		}
		else if (streamFeatures->hasAuthenticationMechanisms()) {
			if (!certificate_.isNull()) {
				if (streamFeatures->hasAuthenticationMechanism("EXTERNAL")) {
					state_ = Authenticating;
					xmppLayer_->writeElement(boost::shared_ptr<Element>(new AuthRequest("EXTERNAL", "")));
				}
				else {
					setError(ClientCertificateError);
				}
			}
			else if (streamFeatures->hasAuthenticationMechanism("PLAIN")) {
				state_ = WaitingForCredentials;
				onNeedCredentials();
			}
			else {
				setError(NoSupportedAuthMechanismsError);
			}
		}
		else {
			// Start the session

			// Add a whitespace ping layer
			whitespacePingLayer_ = boost::shared_ptr<WhitespacePingLayer>(new WhitespacePingLayer());
			streamStack_->addLayer(whitespacePingLayer_);

			if (streamFeatures->hasSession()) {
				needSessionStart_ = true;
			}

			if (streamFeatures->hasResourceBind()) {
				state_ = BindingResource;
				boost::shared_ptr<ResourceBind> resourceBind(new ResourceBind());
				if (!jid_.getResource().isEmpty()) {
					resourceBind->setResource(jid_.getResource());
				}
				xmppLayer_->writeElement(IQ::createRequest(IQ::Set, JID(), "session-bind", resourceBind));
			}
			else if (needSessionStart_) {
				sendSessionStart();
			}
			else {
				state_ = SessionStarted;
				onSessionStarted();
			}
		}
	}
	else if (dynamic_cast<AuthSuccess*>(element.get())) {
		checkState(Authenticating);
		state_ = WaitingForStreamStart;
		xmppLayer_->resetParser();
		sendStreamHeader();
	}
	else if (dynamic_cast<AuthFailure*>(element.get())) {
		setError(AuthenticationFailedError);
	}
	else if (dynamic_cast<TLSProceed*>(element.get())) {
		tlsLayer_ = tlsLayerFactory_->createTLSLayer();
		streamStack_->addLayer(tlsLayer_);
		if (!certificate_.isNull() && !tlsLayer_->setClientCertificate(certificate_)) {
			setError(ClientCertificateLoadError);
		}
		else {
			tlsLayer_->onConnected.connect(boost::bind(&ClientSession::handleTLSConnected, this));
			tlsLayer_->onError.connect(boost::bind(&ClientSession::handleTLSError, this));
			tlsLayer_->connect();
		}
	}
	else if (dynamic_cast<StartTLSFailure*>(element.get())) {
		setError(TLSError);
	}
	else if (IQ* iq = dynamic_cast<IQ*>(element.get())) {
		if (state_ == BindingResource) {
			boost::shared_ptr<ResourceBind> resourceBind(iq->getPayload<ResourceBind>());
			if (iq->getType() == IQ::Error && iq->getID() == "session-bind") {
				setError(ResourceBindError);
			}
			else if (!resourceBind) {
				setError(UnexpectedElementError);
			}
			else if (iq->getType() == IQ::Result) {
				jid_ = resourceBind->getJID();
				if (!jid_.isValid()) {
					setError(ResourceBindError);
				}
				if (needSessionStart_) {
					sendSessionStart();
				}
				else {
					state_ = SessionStarted;
				}
			}
			else {
				setError(UnexpectedElementError);
			}
		}
		else if (state_ == StartingSession) {
			if (iq->getType() == IQ::Result) {
				state_ = SessionStarted;
				onSessionStarted();
			}
			else if (iq->getType() == IQ::Error) {
				setError(SessionStartError);
			}
			else {
				setError(UnexpectedElementError);
			}
		}
		else {
			setError(UnexpectedElementError);
		}
	}
	else {
		// FIXME Not correct?
		state_ = SessionStarted;
		onSessionStarted();
	}
}

void ClientSession::sendSessionStart() {
	state_ = StartingSession;
	xmppLayer_->writeElement(IQ::createRequest(IQ::Set, JID(), "session-start", boost::shared_ptr<StartSession>(new StartSession())));
}

void ClientSession::setError(SessionError error) {
	assert(error != NoError);
	state_ = Error;
	error_ = error;
	onError(error);
}

bool ClientSession::checkState(State state) {
	if (state_ != state) {
		setError(UnexpectedElementError);
		return false;
	}
	return true;
}

void ClientSession::sendCredentials(const String& password) {
	assert(WaitingForCredentials);
	state_ = Authenticating;
	xmppLayer_->writeElement(boost::shared_ptr<Element>(new AuthRequest("PLAIN", PLAINMessage(jid_.getNode(), password).getValue())));
}

void ClientSession::sendElement(boost::shared_ptr<Element> element) {
	assert(SessionStarted);
	xmppLayer_->writeElement(element);
}

void ClientSession::handleTLSConnected() {
	state_ = WaitingForStreamStart;
	xmppLayer_->resetParser();
	sendStreamHeader();
}

void ClientSession::handleTLSError() {
	setError(TLSError);
}

}
