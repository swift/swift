/*
 * Copyright (c) 2010 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#include "Swiften/Client/Client.h"

#include <boost/bind.hpp>

#include "Swiften/Network/MainBoostIOServiceThread.h"
#include "Swiften/Network/BoostIOServiceThread.h"
#include "Swiften/Client/ClientSession.h"
#include "Swiften/StreamStack/PlatformTLSLayerFactory.h"
#include "Swiften/Network/Connector.h"
#include "Swiften/Network/BoostConnectionFactory.h"
#include "Swiften/Network/BoostTimerFactory.h"
#include "Swiften/TLS/PKCS12Certificate.h"
#include "Swiften/Session/BasicSessionStream.h"
#include "Swiften/Queries/IQRouter.h"

namespace Swift {

Client::Client(const JID& jid, const String& password) : jid_(jid), password_(password), disconnectRequested_(false) {
	iqRouter_ = new IQRouter(this);
	connectionFactory_ = new BoostConnectionFactory(&MainBoostIOServiceThread::getInstance().getIOService());
	timerFactory_ = new BoostTimerFactory(&MainBoostIOServiceThread::getInstance().getIOService());
	tlsLayerFactory_ = new PlatformTLSLayerFactory();
}

Client::~Client() {
	if (session_ || connection_) {
		std::cerr << "Warning: Client not disconnected properly" << std::endl;
	}
	delete tlsLayerFactory_;
	delete timerFactory_;
	delete connectionFactory_;
	delete iqRouter_;
}

bool Client::isAvailable() {
	return session_ && session_->getState() == ClientSession::Initialized;
}

void Client::connect() {
	connect(jid_.getDomain());
}

void Client::connect(const JID& jid) {
	jid_ = jid;
	connect();
}

void Client::connect(const String& host) {
	assert(!connector_);
	connector_ = Connector::create(host, &resolver_, connectionFactory_, timerFactory_);
	connector_->onConnectFinished.connect(boost::bind(&Client::handleConnectorFinished, this, _1));
	connector_->setTimeoutMilliseconds(60*1000);
	connector_->start();
}

void Client::handleConnectorFinished(boost::shared_ptr<Connection> connection) {
	connector_->onConnectFinished.disconnect(boost::bind(&Client::handleConnectorFinished, this, _1));
	connector_.reset();
	if (!connection) {
		if (!disconnectRequested_) {
			onError(ClientError::ConnectionError);
		}
	}
	else {
		assert(!connection_);
		connection_ = connection;

		assert(!sessionStream_);
		sessionStream_ = boost::shared_ptr<BasicSessionStream>(new BasicSessionStream(connection_, &payloadParserFactories_, &payloadSerializers_, tlsLayerFactory_, timerFactory_));
		if (!certificate_.isEmpty()) {
			sessionStream_->setTLSCertificate(PKCS12Certificate(certificate_, password_));
		}
		sessionStream_->onDataRead.connect(boost::bind(&Client::handleDataRead, this, _1));
		sessionStream_->onDataWritten.connect(boost::bind(&Client::handleDataWritten, this, _1));
		sessionStream_->initialize();

		session_ = ClientSession::create(jid_, sessionStream_);
		session_->onInitialized.connect(boost::bind(&Client::handleSessionInitialized, this));
		session_->onStanzaAcked.connect(boost::bind(&Client::handleStanzaAcked, this, _1));
		session_->onFinished.connect(boost::bind(&Client::handleSessionFinished, this, _1));
		session_->onNeedCredentials.connect(boost::bind(&Client::handleNeedCredentials, this));
		session_->onStanzaReceived.connect(boost::bind(&Client::handleStanza, this, _1));
		session_->start();
	}
}

void Client::disconnect() {
	// FIXME: We should be able to do without this boolean. We just have to make sure we can tell the difference between
	// connector finishing without a connection due to an error or because of a disconnect.
	disconnectRequested_ = true;
	if (session_) {
		session_->finish();
	}
	else if (connector_) {
		connector_->stop();
		assert(!session_);
	}
	assert(!session_);
	assert(!sessionStream_);
	assert(!connector_);
	disconnectRequested_ = false;
}

void Client::send(boost::shared_ptr<Stanza> stanza) {
	if (!isAvailable()) {
		std::cerr << "Warning: Client: Trying to send a stanza while disconnected." << std::endl;
		return;
	}
	session_->sendStanza(stanza);
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

void Client::handleStanza(boost::shared_ptr<Stanza> stanza) {
	boost::shared_ptr<Message> message = boost::dynamic_pointer_cast<Message>(stanza);
	if (message) {
		onMessageReceived(message);
		return;
	}

	boost::shared_ptr<Presence> presence = boost::dynamic_pointer_cast<Presence>(stanza);
	if (presence) {
		onPresenceReceived(presence);
		return;
	}

	boost::shared_ptr<IQ> iq = boost::dynamic_pointer_cast<IQ>(stanza);
	if (iq) {
		onIQReceived(iq);
		return;
	}
}

void Client::setCertificate(const String& certificate) {
	certificate_ = certificate;
}

void Client::handleSessionFinished(boost::shared_ptr<Error> error) {
	session_->onInitialized.disconnect(boost::bind(&Client::handleSessionInitialized, this));
	session_->onStanzaAcked.disconnect(boost::bind(&Client::handleStanzaAcked, this, _1));
	session_->onFinished.disconnect(boost::bind(&Client::handleSessionFinished, this, _1));
	session_->onNeedCredentials.disconnect(boost::bind(&Client::handleNeedCredentials, this));
	session_->onStanzaReceived.disconnect(boost::bind(&Client::handleStanza, this, _1));
	session_.reset();

	sessionStream_->onDataRead.disconnect(boost::bind(&Client::handleDataRead, this, _1));
	sessionStream_->onDataWritten.disconnect(boost::bind(&Client::handleDataWritten, this, _1));
	sessionStream_.reset();

	connection_->disconnect();
	connection_.reset();

	onAvailableChanged(false);

	if (error) {
		ClientError clientError;
		if (boost::shared_ptr<ClientSession::Error> actualError = boost::dynamic_pointer_cast<ClientSession::Error>(error)) {
			switch(actualError->type) {
				case ClientSession::Error::AuthenticationFailedError:
					clientError = ClientError(ClientError::AuthenticationFailedError);
					break;
				case ClientSession::Error::CompressionFailedError:
					clientError = ClientError(ClientError::CompressionFailedError);
					break;
				case ClientSession::Error::ServerVerificationFailedError:
					clientError = ClientError(ClientError::ServerVerificationFailedError);
					break;
				case ClientSession::Error::NoSupportedAuthMechanismsError:
					clientError = ClientError(ClientError::NoSupportedAuthMechanismsError);
					break;
				case ClientSession::Error::UnexpectedElementError:
					clientError = ClientError(ClientError::UnexpectedElementError);
					break;
				case ClientSession::Error::ResourceBindError:
					clientError = ClientError(ClientError::ResourceBindError);
					break;
				case ClientSession::Error::SessionStartError:
					clientError = ClientError(ClientError::SessionStartError);
					break;
				case ClientSession::Error::TLSError:
					clientError = ClientError(ClientError::TLSError);
					break;
				case ClientSession::Error::TLSClientCertificateError:
					clientError = ClientError(ClientError::ClientCertificateError);
					break;
			}
		}
		else if (boost::shared_ptr<SessionStream::Error> actualError = boost::dynamic_pointer_cast<SessionStream::Error>(error)) {
			switch(actualError->type) {
				case SessionStream::Error::ParseError:
					clientError = ClientError(ClientError::XMLError);
					break;
				case SessionStream::Error::TLSError:
					clientError = ClientError(ClientError::TLSError);
					break;
				case SessionStream::Error::InvalidTLSCertificateError:
					clientError = ClientError(ClientError::ClientCertificateLoadError);
					break;
				case SessionStream::Error::ConnectionReadError:
					clientError = ClientError(ClientError::ConnectionReadError);
					break;
				case SessionStream::Error::ConnectionWriteError:
					clientError = ClientError(ClientError::ConnectionWriteError);
					break;
			}
		}
		onError(clientError);
	}
}

void Client::handleNeedCredentials() {
	assert(session_);
	session_->sendCredentials(password_);
}

bool Client::getStreamManagementEnabled() const {
	if (session_) {
		return session_->getStreamManagementEnabled();
	}
	return false;
}

void Client::handleDataRead(const String& data) {
	onDataRead(data);
}

void Client::handleDataWritten(const String& data) {
	onDataWritten(data);
}

void Client::handleStanzaAcked(boost::shared_ptr<Stanza> stanza) {
	onStanzaAcked(stanza);
}

void Client::handleSessionInitialized() {
	jid_ = session_->getLocalJID();
	onConnected();
	onAvailableChanged(true);
}

}
