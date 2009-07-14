#include "Swiften/Client/Session.h"

#include <boost/bind.hpp>

#include "Swiften/Network/ConnectionFactory.h"
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

Session::Session(const JID& jid, ConnectionFactory* connectionFactory, TLSLayerFactory* tlsLayerFactory, PayloadParserFactoryCollection* payloadParserFactories, PayloadSerializerCollection* payloadSerializers) : 
		jid_(jid), 
		connectionFactory_(connectionFactory), 
		tlsLayerFactory_(tlsLayerFactory),
		payloadParserFactories_(payloadParserFactories),
		payloadSerializers_(payloadSerializers),
		state_(Initial), 
		error_(NoError),
		xmppLayer_(0), 
		tlsLayer_(0),
		connectionLayer_(0), 
		whitespacePingLayer_(0),
		streamStack_(0),
		needSessionStart_(false) {
}

Session::~Session() {
	delete streamStack_;
	delete whitespacePingLayer_;
	delete connectionLayer_;
	delete tlsLayer_;
	delete xmppLayer_;
}

void Session::start() {
	assert(state_ == Initial);
	state_ = Connecting;
	connection_ = connectionFactory_->createConnection();
	connection_->onConnected.connect(boost::bind(&Session::handleConnected, this));
	connection_->onError.connect(boost::bind(&Session::handleConnectionError, this, _1));
	connection_->connect(jid_.getDomain());
}

void Session::stop() {
	// TODO: Send end stream header if applicable
	connection_->disconnect();
}

void Session::handleConnected() {
	assert(state_ == Connecting);
	initializeStreamStack();
	state_ = WaitingForStreamStart;
	sendStreamHeader();
}

void Session::sendStreamHeader() {
	xmppLayer_->writeHeader(jid_.getDomain());
}

void Session::initializeStreamStack() {
	xmppLayer_ = new XMPPLayer(payloadParserFactories_, payloadSerializers_);
	xmppLayer_->onStreamStart.connect(boost::bind(&Session::handleStreamStart, this));
	xmppLayer_->onElement.connect(boost::bind(&Session::handleElement, this, _1));
	xmppLayer_->onError.connect(boost::bind(&Session::setError, this, XMLError));
	xmppLayer_->onDataRead.connect(boost::bind(boost::ref(onDataRead), _1));
	xmppLayer_->onWriteData.connect(boost::bind(boost::ref(onDataWritten), _1));
	connectionLayer_ = new ConnectionLayer(connection_);
	streamStack_ = new StreamStack(xmppLayer_, connectionLayer_);
}

void Session::handleConnectionError(Connection::Error error) {
	switch (error) {
		case Connection::DomainNameResolveError:
			setError(DomainNameResolveError);
			break;
		case Connection::ReadError:
			setError(ConnectionReadError);
			break;
		case Connection::WriteError:
			setError(ConnectionWriteError);
			break;
		case Connection::ConnectionError:
			setError(ConnectionError);
			break;
	}
}

void Session::setCertificate(const PKCS12Certificate& certificate) {
	certificate_ = certificate;
}

void Session::handleStreamStart() {
	checkState(WaitingForStreamStart);
	state_ = Negotiating;
}

void Session::handleElement(boost::shared_ptr<Element> element) {
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
			whitespacePingLayer_ = new WhitespacePingLayer();
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
			tlsLayer_->onConnected.connect(boost::bind(&Session::handleTLSConnected, this));
			tlsLayer_->onError.connect(boost::bind(&Session::handleTLSError, this));
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

void Session::sendSessionStart() {
	state_ = StartingSession;
	xmppLayer_->writeElement(IQ::createRequest(IQ::Set, JID(), "session-start", boost::shared_ptr<StartSession>(new StartSession())));
}

void Session::setError(SessionError error) {
	assert(error != NoError);
	state_ = Error;
	error_ = error;
	onError(error);
}

bool Session::checkState(State state) {
	if (state_ != state) {
		setError(UnexpectedElementError);
		return false;
	}
	return true;
}

void Session::sendCredentials(const String& password) {
	assert(WaitingForCredentials);
	state_ = Authenticating;
	xmppLayer_->writeElement(boost::shared_ptr<Element>(new AuthRequest("PLAIN", PLAINMessage(jid_.getNode(), password).getValue())));
}

void Session::sendElement(boost::shared_ptr<Element> element) {
	assert(SessionStarted);
	xmppLayer_->writeElement(element);
}

void Session::handleTLSConnected() {
	state_ = WaitingForStreamStart;
	xmppLayer_->resetParser();
	sendStreamHeader();
}

void Session::handleTLSError() {
	setError(TLSError);
}

}
