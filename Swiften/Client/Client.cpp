#include "Swiften/Client/Client.h"

#include <boost/bind.hpp>

#include "Swiften/Network/DomainNameResolver.h"
#include "Swiften/Client/Session.h"
#include "Swiften/StreamStack/PlatformTLSLayerFactory.h"
#include "Swiften/Network/BoostConnectionFactory.h"
#include "Swiften/Network/DomainNameResolveException.h"
#include "Swiften/TLS/PKCS12Certificate.h"

namespace Swift {

Client::Client(const JID& jid, const String& password) :
		IQRouter(this), jid_(jid), password_(password), session_(0) {
	connectionFactory_ = new BoostConnectionFactory(&boostIOServiceThread_.getIOService());
	tlsLayerFactory_ = new PlatformTLSLayerFactory();
}

Client::~Client() {
	delete session_;
	delete tlsLayerFactory_;
	delete connectionFactory_;
}

void Client::connect() {
	delete session_;
	session_ = 0;

	DomainNameResolver resolver;
	try {
		HostAddressPort remote = resolver.resolve(jid_.getDomain().getUTF8String());
		connection_ = connectionFactory_->createConnection();
		connection_->onConnectFinished.connect(boost::bind(&Client::handleConnectionConnectFinished, this, _1));
		connection_->connect(remote);
	}
	catch (const DomainNameResolveException& e) {
		onError(ClientError::DomainNameResolveError);
	}
}

void Client::handleConnectionConnectFinished(bool error) {
	if (error) {
		onError(ClientError::ConnectionError);
	}
	else {
		session_ = new Session(jid_, connection_, tlsLayerFactory_, &payloadParserFactories_, &payloadSerializers_);
		if (!certificate_.isEmpty()) {
			session_->setCertificate(PKCS12Certificate(certificate_, password_));
		}
		session_->onSessionStarted.connect(boost::bind(boost::ref(onConnected)));
		session_->onError.connect(boost::bind(&Client::handleSessionError, this, _1));
		session_->onNeedCredentials.connect(boost::bind(&Client::handleNeedCredentials, this));
		session_->onDataRead.connect(boost::bind(&Client::handleDataRead, this, _1));
		session_->onDataWritten.connect(boost::bind(&Client::handleDataWritten, this, _1));
		session_->onElementReceived.connect(boost::bind(&Client::handleElement, this, _1));
		session_->start();
	}
}

void Client::disconnect() {
	if (session_) {
		session_->stop();
	}
}

void Client::send(boost::shared_ptr<Stanza> stanza) {
	session_->sendElement(stanza);
}

void Client::sendIQ(boost::shared_ptr<IQ> iq) {
	send(iq);
}

void Client::sendMessage(boost::shared_ptr<Message> message) {
	send(message);
}

void Client::sendPresence(boost::shared_ptr<Presence> presence) {
	send(presence);
}

String Client::getNewIQID() {
	return idGenerator_.generateID();
}

void Client::handleElement(boost::shared_ptr<Element> element) {
	boost::shared_ptr<Message> message = boost::dynamic_pointer_cast<Message>(element);
	if (message) {
		onMessageReceived(message);
		return;
	}

	boost::shared_ptr<Presence> presence = boost::dynamic_pointer_cast<Presence>(element);
	if (presence) {
		onPresenceReceived(presence);
		return;
	}

	boost::shared_ptr<IQ> iq = boost::dynamic_pointer_cast<IQ>(element);
	if (iq) {
		onIQReceived(iq);
		return;
	}
}

void Client::setCertificate(const String& certificate) {
	certificate_ = certificate;
}

void Client::handleSessionError(Session::SessionError error) {
	ClientError clientError;
	switch (error) {
		case Session::NoError: 
			assert(false);
			break;
		case Session::ConnectionReadError:
			clientError = ClientError(ClientError::ConnectionReadError);
			break;
		case Session::ConnectionWriteError:
			clientError = ClientError(ClientError::ConnectionWriteError);
			break;
		case Session::XMLError:
			clientError = ClientError(ClientError::XMLError);
			break;
		case Session::AuthenticationFailedError:
			clientError = ClientError(ClientError::AuthenticationFailedError);
			break;
		case Session::NoSupportedAuthMechanismsError:
			clientError = ClientError(ClientError::NoSupportedAuthMechanismsError);
			break;
		case Session::UnexpectedElementError:
			clientError = ClientError(ClientError::UnexpectedElementError);
			break;
		case Session::ResourceBindError:
			clientError = ClientError(ClientError::ResourceBindError);
			break;
		case Session::SessionStartError:
			clientError = ClientError(ClientError::SessionStartError);
			break;
		case Session::TLSError:
			clientError = ClientError(ClientError::TLSError);
			break;
		case Session::ClientCertificateLoadError:
			clientError = ClientError(ClientError::ClientCertificateLoadError);
			break;
		case Session::ClientCertificateError:
			clientError = ClientError(ClientError::ClientCertificateError);
			break;
	}
	onError(clientError);
}

void Client::handleNeedCredentials() {
	session_->sendCredentials(password_);
}

void Client::handleDataRead(const ByteArray& data) {
	onDataRead(String(data.getData(), data.getSize()));
}

void Client::handleDataWritten(const ByteArray& data) {
	onDataWritten(String(data.getData(), data.getSize()));
}

}
