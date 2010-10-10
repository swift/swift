/*
 * Copyright (c) 2010 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#include "Swiften/Client/CoreClient.h"

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
#include "Swiften/Base/IDGenerator.h"
#include "Swiften/Client/ClientSessionStanzaChannel.h"

namespace Swift {

CoreClient::CoreClient(const JID& jid, const String& password) : jid_(jid), password_(password), disconnectRequested_(false) {
	stanzaChannel_ = new ClientSessionStanzaChannel();
	stanzaChannel_->onMessageReceived.connect(boost::ref(onMessageReceived));
	stanzaChannel_->onPresenceReceived.connect(boost::ref(onPresenceReceived));
	stanzaChannel_->onStanzaAcked.connect(boost::ref(onStanzaAcked));
	stanzaChannel_->onAvailableChanged.connect(boost::bind(&CoreClient::handleStanzaChannelAvailableChanged, this, _1));

	iqRouter_ = new IQRouter(stanzaChannel_);
	connectionFactory_ = new BoostConnectionFactory(&MainBoostIOServiceThread::getInstance().getIOService());
	timerFactory_ = new BoostTimerFactory(&MainBoostIOServiceThread::getInstance().getIOService());
	tlsLayerFactory_ = new PlatformTLSLayerFactory();
}

CoreClient::~CoreClient() {
	if (session_ || connection_) {
		std::cerr << "Warning: Client not disconnected properly" << std::endl;
	}
	delete tlsLayerFactory_;
	delete timerFactory_;
	delete connectionFactory_;
	delete iqRouter_;

	stanzaChannel_->onAvailableChanged.disconnect(boost::bind(&CoreClient::handleStanzaChannelAvailableChanged, this, _1));
	stanzaChannel_->onMessageReceived.disconnect(boost::ref(onMessageReceived));
	stanzaChannel_->onPresenceReceived.disconnect(boost::ref(onPresenceReceived));
	stanzaChannel_->onStanzaAcked.disconnect(boost::ref(onStanzaAcked));
	delete stanzaChannel_;
}

void CoreClient::connect() {
	connect(jid_.getDomain());
}

void CoreClient::connect(const JID& jid) {
	jid_ = jid;
	connect();
}

void CoreClient::connect(const String& host) {
	assert(!connector_);
	connector_ = Connector::create(host, &resolver_, connectionFactory_, timerFactory_);
	connector_->onConnectFinished.connect(boost::bind(&CoreClient::handleConnectorFinished, this, _1));
	connector_->setTimeoutMilliseconds(60*1000);
	connector_->start();
}

void CoreClient::handleConnectorFinished(boost::shared_ptr<Connection> connection) {
	connector_->onConnectFinished.disconnect(boost::bind(&CoreClient::handleConnectorFinished, this, _1));
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
		sessionStream_->onDataRead.connect(boost::bind(&CoreClient::handleDataRead, this, _1));
		sessionStream_->onDataWritten.connect(boost::bind(&CoreClient::handleDataWritten, this, _1));
		sessionStream_->initialize();

		session_ = ClientSession::create(jid_, sessionStream_);
		stanzaChannel_->setSession(session_);
		session_->onFinished.connect(boost::bind(&CoreClient::handleSessionFinished, this, _1));
		session_->onNeedCredentials.connect(boost::bind(&CoreClient::handleNeedCredentials, this));
		session_->start();
	}
}

void CoreClient::disconnect() {
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

void CoreClient::setCertificate(const String& certificate) {
	certificate_ = certificate;
}

void CoreClient::handleSessionFinished(boost::shared_ptr<Error> error) {
	session_->onFinished.disconnect(boost::bind(&CoreClient::handleSessionFinished, this, _1));
	session_->onNeedCredentials.disconnect(boost::bind(&CoreClient::handleNeedCredentials, this));
	session_.reset();

	sessionStream_->onDataRead.disconnect(boost::bind(&CoreClient::handleDataRead, this, _1));
	sessionStream_->onDataWritten.disconnect(boost::bind(&CoreClient::handleDataWritten, this, _1));
	sessionStream_.reset();

	connection_->disconnect();
	connection_.reset();

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

void CoreClient::handleNeedCredentials() {
	assert(session_);
	session_->sendCredentials(password_);
}

void CoreClient::handleDataRead(const String& data) {
	onDataRead(data);
}

void CoreClient::handleDataWritten(const String& data) {
	onDataWritten(data);
}

void CoreClient::handleStanzaChannelAvailableChanged(bool available) {
	if (available) {
		onConnected();
	}
}

void CoreClient::sendMessage(boost::shared_ptr<Message> message) {
	stanzaChannel_->sendMessage(message);
}

void CoreClient::sendPresence(boost::shared_ptr<Presence> presence) {
	stanzaChannel_->sendPresence(presence);
}

}
