/*
 * Copyright (c) 2010-2011 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#include <Swiften/Client/CoreClient.h>

#include <boost/bind.hpp>
#include <boost/smart_ptr/make_shared.hpp>

#include <Swiften/Base/IDGenerator.h>
#include <Swiften/Base/Log.h>
#include <Swiften/Base/foreach.h>
#include <Swiften/Base/Algorithm.h>
#include <Swiften/Client/ClientSession.h>
#include <Swiften/TLS/CertificateVerificationError.h>
#include <Swiften/TLS/TLSError.h>
#include <Swiften/Network/ChainedConnector.h>
#include <Swiften/Network/NetworkFactories.h>
#include <Swiften/Network/ProxyProvider.h>
#include <Swiften/Network/DomainNameResolveError.h>
#include <Swiften/TLS/PKCS12Certificate.h>
#include <Swiften/Session/BasicSessionStream.h>
#include <Swiften/Session/BOSHSessionStream.h>
#include <Swiften/Queries/IQRouter.h>
#include <Swiften/Client/ClientSessionStanzaChannel.h>
#include <Swiften/Network/SOCKS5ProxiedConnectionFactory.h>
#include <Swiften/Network/HTTPConnectProxiedConnectionFactory.h>

namespace Swift {

CoreClient::CoreClient(const JID& jid, const SafeByteArray& password, NetworkFactories* networkFactories) : jid_(jid), password_(password), networkFactories(networkFactories), disconnectRequested_(false), certificateTrustChecker(NULL) {
	stanzaChannel_ = new ClientSessionStanzaChannel();
	stanzaChannel_->onMessageReceived.connect(boost::bind(&CoreClient::handleMessageReceived, this, _1));
	stanzaChannel_->onPresenceReceived.connect(boost::bind(&CoreClient::handlePresenceReceived, this, _1));
	stanzaChannel_->onStanzaAcked.connect(boost::bind(&CoreClient::handleStanzaAcked, this, _1));
	stanzaChannel_->onAvailableChanged.connect(boost::bind(&CoreClient::handleStanzaChannelAvailableChanged, this, _1));

	iqRouter_ = new IQRouter(stanzaChannel_);
	iqRouter_->setJID(jid);
}

CoreClient::~CoreClient() {
	forceReset();
	delete iqRouter_;

	stanzaChannel_->onAvailableChanged.disconnect(boost::bind(&CoreClient::handleStanzaChannelAvailableChanged, this, _1));
	stanzaChannel_->onMessageReceived.disconnect(boost::bind(&CoreClient::handleMessageReceived, this, _1));
	stanzaChannel_->onPresenceReceived.disconnect(boost::bind(&CoreClient::handlePresenceReceived, this, _1));
	stanzaChannel_->onStanzaAcked.disconnect(boost::bind(&CoreClient::handleStanzaAcked, this, _1));
	delete stanzaChannel_;
}

void CoreClient::connect(const ClientOptions& o) {
	SWIFT_LOG(debug) << "Connecting" << std::endl;
	options = o;
	connect(jid_.getDomain());
}

void CoreClient::connect(const std::string& host) {
	forceReset();

	SWIFT_LOG(debug) << "Connecting to host " << host << std::endl;
	disconnectRequested_ = false;
	assert(!connector_);
	assert(proxyConnectionFactories.empty());
	if (networkFactories->getProxyProvider()->getSOCKS5Proxy().isValid()) {
		proxyConnectionFactories.push_back(new SOCKS5ProxiedConnectionFactory(networkFactories->getConnectionFactory(), networkFactories->getProxyProvider()->getSOCKS5Proxy()));
	}
	if(networkFactories->getProxyProvider()->getHTTPConnectProxy().isValid()) {
		proxyConnectionFactories.push_back(new HTTPConnectProxiedConnectionFactory(networkFactories->getDomainNameResolver(), networkFactories->getConnectionFactory(), networkFactories->getTimerFactory(), networkFactories->getEventLoop(), networkFactories->getProxyProvider()->getHTTPConnectProxy().getAddress().toString(), networkFactories->getProxyProvider()->getHTTPConnectProxy().getPort()));
	}
	std::vector<ConnectionFactory*> connectionFactories(proxyConnectionFactories);
	if (options.boshURL.empty()) {
		connectionFactories.push_back(networkFactories->getConnectionFactory());
		connector_ = boost::make_shared<ChainedConnector>(host, networkFactories->getDomainNameResolver(), connectionFactories, networkFactories->getTimerFactory());
		connector_->onConnectFinished.connect(boost::bind(&CoreClient::handleConnectorFinished, this, _1, _2));
		connector_->setTimeoutMilliseconds(60*1000);
		connector_->start();
	}
	else {
		/* Autodiscovery of which proxy works is largely ok with a TCP session, because this is a one-off. With BOSH
		 * it would be quite painful given that potentially every stanza could be sent on a new connection.
		 */
		//sessionStream_ = boost::make_shared<BOSHSessionStream>(boost::make_shared<BOSHConnectionFactory>(options.boshURL, networkFactories->getConnectionFactory(), networkFactories->getXMLParserFactory(), networkFactories->getTLSContextFactory()), getPayloadParserFactories(), getPayloadSerializers(), networkFactories->getTLSContextFactory(), networkFactories->getTimerFactory(), networkFactories->getXMLParserFactory(), networkFactories->getEventLoop(), host, options.boshHTTPConnectProxyURL, options.boshHTTPConnectProxyAuthID, options.boshHTTPConnectProxyAuthPassword);
		sessionStream_ = boost::shared_ptr<BOSHSessionStream>(new BOSHSessionStream(
				options.boshURL,
				getPayloadParserFactories(),
				getPayloadSerializers(),
				networkFactories->getConnectionFactory(),
				networkFactories->getTLSContextFactory(),
				networkFactories->getTimerFactory(),
				networkFactories->getXMLParserFactory(),
				networkFactories->getEventLoop(),
				networkFactories->getDomainNameResolver(),
				host,
				options.boshHTTPConnectProxyURL,
				options.boshHTTPConnectProxyAuthID,
				options.boshHTTPConnectProxyAuthPassword));
		sessionStream_->onDataRead.connect(boost::bind(&CoreClient::handleDataRead, this, _1));
		sessionStream_->onDataWritten.connect(boost::bind(&CoreClient::handleDataWritten, this, _1));
		bindSessionToStream();
	}

}

void CoreClient::bindSessionToStream() {
	session_ = ClientSession::create(jid_, sessionStream_);
	session_->setCertificateTrustChecker(certificateTrustChecker);
	session_->setUseStreamCompression(options.useStreamCompression);
	session_->setAllowPLAINOverNonTLS(options.allowPLAINWithoutTLS);
	switch(options.useTLS) {
		case ClientOptions::UseTLSWhenAvailable:
			session_->setUseTLS(ClientSession::UseTLSWhenAvailable);
			break;
		case ClientOptions::NeverUseTLS:
			session_->setUseTLS(ClientSession::NeverUseTLS);
			break;
		case ClientOptions::RequireTLS:
			session_->setUseTLS(ClientSession::RequireTLS);
			break;
	}
	session_->setUseAcks(options.useAcks);
	stanzaChannel_->setSession(session_);
	session_->onFinished.connect(boost::bind(&CoreClient::handleSessionFinished, this, _1));
	session_->onNeedCredentials.connect(boost::bind(&CoreClient::handleNeedCredentials, this));
	session_->start();
}

/**
 * Only called for TCP sessions. BOSH is handled inside the BOSHSessionStream.
 */
void CoreClient::handleConnectorFinished(boost::shared_ptr<Connection> connection, boost::shared_ptr<Error> error) {
	resetConnector();
	if (!connection) {
		if (options.forgetPassword) {
			purgePassword();
		}
		boost::optional<ClientError> clientError;
		if (!disconnectRequested_) {
			clientError = boost::dynamic_pointer_cast<DomainNameResolveError>(error) ? boost::optional<ClientError>(ClientError::DomainNameResolveError) : boost::optional<ClientError>(ClientError::ConnectionError);
		}
		onDisconnected(clientError);
	}
	else {
		assert(!connection_);
		connection_ = connection;

		assert(!sessionStream_);
		sessionStream_ = boost::make_shared<BasicSessionStream>(ClientStreamType, connection_, getPayloadParserFactories(), getPayloadSerializers(), networkFactories->getTLSContextFactory(), networkFactories->getTimerFactory(), networkFactories->getXMLParserFactory());
		if (certificate_ && !certificate_->isNull()) {
			sessionStream_->setTLSCertificate(certificate_);
		}
		sessionStream_->onDataRead.connect(boost::bind(&CoreClient::handleDataRead, this, _1));
		sessionStream_->onDataWritten.connect(boost::bind(&CoreClient::handleDataWritten, this, _1));

		bindSessionToStream();
	}
}

void CoreClient::disconnect() {
	// FIXME: We should be able to do without this boolean. We just have to make sure we can tell the difference between
	// connector finishing without a connection due to an error or because of a disconnect.
	disconnectRequested_ = true;
	if (session_ && !session_->isFinished()) {
		session_->finish();
	}
	else if (connector_) {
		connector_->stop();
	}
}

void CoreClient::setCertificate(CertificateWithKey::ref certificate) {
	certificate_ = certificate;
}

void CoreClient::handleSessionFinished(boost::shared_ptr<Error> error) {
	if (options.forgetPassword) {
		purgePassword();
	}
	resetSession();

	boost::optional<ClientError> actualError;
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
				case ClientSession::Error::StreamError:
					clientError = ClientError(ClientError::StreamError);
					break;
			}
		}
		else if (boost::shared_ptr<TLSError> actualError = boost::dynamic_pointer_cast<TLSError>(error)) {
			switch(actualError->getType()) {
				case TLSError::CertificateCardRemoved:
					clientError = ClientError(ClientError::CertificateCardRemoved);
					break;
				default:
					clientError = ClientError(ClientError::TLSError);
					break;
			}
		}
		else if (boost::shared_ptr<SessionStream::SessionStreamError> actualError = boost::dynamic_pointer_cast<SessionStream::SessionStreamError>(error)) {
			switch(actualError->type) {
				case SessionStream::SessionStreamError::ParseError:
					clientError = ClientError(ClientError::XMLError);
					break;
				case SessionStream::SessionStreamError::TLSError:
					clientError = ClientError(ClientError::TLSError);
					break;
				case SessionStream::SessionStreamError::InvalidTLSCertificateError:
					clientError = ClientError(ClientError::ClientCertificateLoadError);
					break;
				case SessionStream::SessionStreamError::ConnectionReadError:
					clientError = ClientError(ClientError::ConnectionReadError);
					break;
				case SessionStream::SessionStreamError::ConnectionWriteError:
					clientError = ClientError(ClientError::ConnectionWriteError);
					break;
			}
		}
		else if (boost::shared_ptr<CertificateVerificationError> verificationError = boost::dynamic_pointer_cast<CertificateVerificationError>(error)) {
			switch(verificationError->getType()) {
				case CertificateVerificationError::UnknownError: 
					clientError = ClientError(ClientError::UnknownCertificateError);
					break;
				case CertificateVerificationError::Expired: 
					clientError = ClientError(ClientError::CertificateExpiredError);
					break;
				case CertificateVerificationError::NotYetValid: 
					clientError = ClientError(ClientError::CertificateNotYetValidError);
					break;
				case CertificateVerificationError::SelfSigned: 
					clientError = ClientError(ClientError::CertificateSelfSignedError);
					break;
				case CertificateVerificationError::Rejected: 
					clientError = ClientError(ClientError::CertificateRejectedError);
					break;
				case CertificateVerificationError::Untrusted: 
					clientError = ClientError(ClientError::CertificateUntrustedError);
					break;
				case CertificateVerificationError::InvalidPurpose: 
					clientError = ClientError(ClientError::InvalidCertificatePurposeError);
					break;
				case CertificateVerificationError::PathLengthExceeded: 
					clientError = ClientError(ClientError::CertificatePathLengthExceededError);
					break;
				case CertificateVerificationError::InvalidSignature: 
					clientError = ClientError(ClientError::InvalidCertificateSignatureError);
					break;
				case CertificateVerificationError::InvalidCA: 
					clientError = ClientError(ClientError::InvalidCAError);
					break;
				case CertificateVerificationError::InvalidServerIdentity:
					clientError = ClientError(ClientError::InvalidServerIdentityError);
					break;
				case CertificateVerificationError::Revoked:
					clientError = ClientError(ClientError::RevokedError);
					break;
				case CertificateVerificationError::RevocationCheckFailed:
					clientError = ClientError(ClientError::RevocationCheckFailedError);
					break;
			}
		}
		actualError = boost::optional<ClientError>(clientError);
	}
	onDisconnected(actualError);
}

void CoreClient::handleNeedCredentials() {
	assert(session_);
	session_->sendCredentials(password_);
	if (options.forgetPassword) {
		purgePassword();
	}
}

void CoreClient::handleDataRead(const SafeByteArray& data) {
	onDataRead(data);
}

void CoreClient::handleDataWritten(const SafeByteArray& data) {
	onDataWritten(data);
}

void CoreClient::handleStanzaChannelAvailableChanged(bool available) {
	if (available) {
		iqRouter_->setJID(session_->getLocalJID());
		handleConnected();
		onConnected();
	}
}

void CoreClient::sendMessage(boost::shared_ptr<Message> message) {
	stanzaChannel_->sendMessage(message);
}

void CoreClient::sendPresence(boost::shared_ptr<Presence> presence) {
	stanzaChannel_->sendPresence(presence);
}

void CoreClient::sendData(const std::string& data) {
	sessionStream_->writeData(data);
}

bool CoreClient::isActive() const {
	return (session_ && !session_->isFinished()) || connector_;
}

void CoreClient::setCertificateTrustChecker(CertificateTrustChecker* checker) {
	certificateTrustChecker = checker;
}


void CoreClient::handlePresenceReceived(Presence::ref presence) {
	onPresenceReceived(presence);
}

void CoreClient::handleMessageReceived(Message::ref message) {
	onMessageReceived(message);
}

void CoreClient::handleStanzaAcked(Stanza::ref stanza) {
	onStanzaAcked(stanza);
}

bool CoreClient::isAvailable() const {
	return stanzaChannel_->isAvailable();
}

bool CoreClient::getStreamManagementEnabled() const {
	return stanzaChannel_->getStreamManagementEnabled();
}

StanzaChannel* CoreClient::getStanzaChannel() const {
	return stanzaChannel_;
}

const JID& CoreClient::getJID() const {
	if (session_) {
		return session_->getLocalJID();
	}
	else {
		return jid_;
	}
}

void CoreClient::purgePassword() {
	safeClear(password_);
}

void CoreClient::resetConnector() {
	connector_->onConnectFinished.disconnect(boost::bind(&CoreClient::handleConnectorFinished, this, _1, _2));
	connector_.reset();
	foreach(ConnectionFactory* f, proxyConnectionFactories) {
		delete f;
	}
	proxyConnectionFactories.clear();
}

void CoreClient::resetSession() {
	session_->onFinished.disconnect(boost::bind(&CoreClient::handleSessionFinished, this, _1));
	session_->onNeedCredentials.disconnect(boost::bind(&CoreClient::handleNeedCredentials, this));

	sessionStream_->onDataRead.disconnect(boost::bind(&CoreClient::handleDataRead, this, _1));
	sessionStream_->onDataWritten.disconnect(boost::bind(&CoreClient::handleDataWritten, this, _1));

	if (connection_) {
		connection_->disconnect();
	}
	else if (boost::dynamic_pointer_cast<BOSHSessionStream>(sessionStream_)) {
		sessionStream_->close();
	}
	sessionStream_.reset();
	connection_.reset();
}

void CoreClient::forceReset() {
	if (connector_) {
		std::cerr << "Warning: Client not disconnected properly: Connector still active" << std::endl;
		resetConnector();
	}
	if (sessionStream_ || connection_) {
		std::cerr << "Warning: Client not disconnected properly: Session still active" << std::endl;
		resetSession();
	}
}

}
