/*
 * Copyright (c) 2010-2016 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#include <Swiften/Client/CoreClient.h>

#include <memory>

#include <boost/bind.hpp>
#include <boost/optional.hpp>

#include <Swiften/Base/Algorithm.h>
#include <Swiften/Base/IDGenerator.h>
#include <Swiften/Base/Log.h>
#include <Swiften/Client/ClientSession.h>
#include <Swiften/Client/ClientSessionStanzaChannel.h>
#include <Swiften/Network/ChainedConnector.h>
#include <Swiften/Network/DomainNameResolveError.h>
#include <Swiften/Network/HTTPConnectProxiedConnectionFactory.h>
#include <Swiften/Network/NetworkFactories.h>
#include <Swiften/Network/ProxyProvider.h>
#include <Swiften/Network/SOCKS5ProxiedConnectionFactory.h>
#include <Swiften/Queries/IQRouter.h>
#include <Swiften/Session/BOSHSessionStream.h>
#include <Swiften/Session/BasicSessionStream.h>
#include <Swiften/TLS/CertificateVerificationError.h>
#include <Swiften/TLS/PKCS12Certificate.h>
#include <Swiften/TLS/TLSError.h>

namespace Swift {

CoreClient::CoreClient(const JID& jid, const SafeByteArray& password, NetworkFactories* networkFactories) : jid_(jid), password_(password), networkFactories(networkFactories), disconnectRequested_(false), certificateTrustChecker(nullptr) {
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
    SWIFT_LOG(debug) << "Connecting ";

    forceReset();
    disconnectRequested_ = false;

    options = o;


    // Determine connection types to use
    assert(proxyConnectionFactories.empty());
    bool useDirectConnection = true;
    HostAddressPort systemSOCKS5Proxy = networkFactories->getProxyProvider()->getSOCKS5Proxy();
    HostAddressPort systemHTTPConnectProxy = networkFactories->getProxyProvider()->getHTTPConnectProxy();
    switch (o.proxyType) {
        case ClientOptions::NoProxy:
            SWIFT_LOG(debug) << " without a proxy" << std::endl;
            break;
        case ClientOptions::SystemConfiguredProxy:
            SWIFT_LOG(debug) << " with a system configured proxy" << std::endl;
            if (systemSOCKS5Proxy.isValid()) {
                SWIFT_LOG(debug) << "Found SOCK5 Proxy: " << systemSOCKS5Proxy.getAddress().toString() << ":" << systemSOCKS5Proxy.getPort() << std::endl;
                proxyConnectionFactories.push_back(new SOCKS5ProxiedConnectionFactory(networkFactories->getDomainNameResolver(), networkFactories->getConnectionFactory(), networkFactories->getTimerFactory(), systemSOCKS5Proxy.getAddress().toString(), systemSOCKS5Proxy.getPort()));
            }
            if (systemHTTPConnectProxy.isValid()) {
                SWIFT_LOG(debug) << "Found HTTPConnect Proxy: " << systemHTTPConnectProxy.getAddress().toString() << ":" << systemHTTPConnectProxy.getPort() << std::endl;
                proxyConnectionFactories.push_back(new HTTPConnectProxiedConnectionFactory(networkFactories->getDomainNameResolver(), networkFactories->getConnectionFactory(), networkFactories->getTimerFactory(), systemHTTPConnectProxy.getAddress().toString(), systemHTTPConnectProxy.getPort()));
            }
            break;
        case ClientOptions::SOCKS5Proxy: {
            SWIFT_LOG(debug) << " with manual configured SOCKS5 proxy" << std::endl;
            std::string proxyHostname = o.manualProxyHostname.empty() ? systemSOCKS5Proxy.getAddress().toString() : o.manualProxyHostname;
            int proxyPort = o.manualProxyPort == -1 ? systemSOCKS5Proxy.getPort() : o.manualProxyPort;
            SWIFT_LOG(debug) << "Proxy: " << proxyHostname << ":" << proxyPort << std::endl;
            proxyConnectionFactories.push_back(new SOCKS5ProxiedConnectionFactory(networkFactories->getDomainNameResolver(), networkFactories->getConnectionFactory(), networkFactories->getTimerFactory(), proxyHostname, proxyPort));
            useDirectConnection = false;
            break;
        }
        case ClientOptions::HTTPConnectProxy: {
            SWIFT_LOG(debug) << " with manual configured HTTPConnect proxy" << std::endl;
            std::string proxyHostname = o.manualProxyHostname.empty() ? systemHTTPConnectProxy.getAddress().toString() : o.manualProxyHostname;
            int proxyPort = o.manualProxyPort == -1 ? systemHTTPConnectProxy.getPort() : o.manualProxyPort;
            SWIFT_LOG(debug) << "Proxy: " << proxyHostname << ":" << proxyPort << std::endl;
            proxyConnectionFactories.push_back(new HTTPConnectProxiedConnectionFactory(networkFactories->getDomainNameResolver(), networkFactories->getConnectionFactory(), networkFactories->getTimerFactory(), proxyHostname, proxyPort, o.httpTrafficFilter));
            useDirectConnection = false;
            break;
        }
    }
    std::vector<ConnectionFactory*> connectionFactories(proxyConnectionFactories);
    if (useDirectConnection) {
        connectionFactories.push_back(networkFactories->getConnectionFactory());
    }

    // Create connector
    std::string host = o.manualHostname.empty() ?  jid_.getDomain() : o.manualHostname;
    int port = o.manualPort;
    boost::optional<std::string> serviceLookupPrefix;
    if (o.manualHostname.empty()) {
        serviceLookupPrefix = "_xmpp-client._tcp.";
    }
    assert(!connector_);
    if (options.boshURL.isEmpty()) {
        connector_ = std::make_shared<ChainedConnector>(host, port, serviceLookupPrefix, networkFactories->getDomainNameResolver(), connectionFactories, networkFactories->getTimerFactory());
        connector_->onConnectFinished.connect(boost::bind(&CoreClient::handleConnectorFinished, this, _1, _2));
        connector_->setTimeoutMilliseconds(2*60*1000);
        connector_->start();
    }
    else {
        /* Autodiscovery of which proxy works is largely ok with a TCP session, because this is a one-off. With BOSH
         * it would be quite painful given that potentially every stanza could be sent on a new connection.
         */
        std::shared_ptr<BOSHSessionStream> boshSessionStream_ = std::shared_ptr<BOSHSessionStream>(new BOSHSessionStream(
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
            options.boshHTTPConnectProxyAuthPassword,
            options.tlsOptions,
            options.httpTrafficFilter));
        sessionStream_ = boshSessionStream_;
        sessionStream_->onDataRead.connect(boost::bind(&CoreClient::handleDataRead, this, _1));
        sessionStream_->onDataWritten.connect(boost::bind(&CoreClient::handleDataWritten, this, _1));
        if (certificate_ && !certificate_->isNull()) {
            SWIFT_LOG(debug) << "set certificate" << std::endl;
            sessionStream_->setTLSCertificate(certificate_);
        }
        boshSessionStream_->open();
        bindSessionToStream();
    }

}

void CoreClient::bindSessionToStream() {
    session_ = ClientSession::create(jid_, sessionStream_, networkFactories->getIDNConverter(), networkFactories->getCryptoProvider(), networkFactories->getTimerFactory());
    session_->setCertificateTrustChecker(certificateTrustChecker);
    session_->setUseStreamCompression(options.useStreamCompression);
    session_->setAllowPLAINOverNonTLS(options.allowPLAINWithoutTLS);
    session_->setSingleSignOn(options.singleSignOn);
    session_->setAuthenticationPort(options.manualPort);
    session_->setSessionShutdownTimeout(options.sessionShutdownTimeoutInMilliseconds);
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
void CoreClient::handleConnectorFinished(std::shared_ptr<Connection> connection, std::shared_ptr<Error> error) {
    resetConnector();
    if (!connection) {
        if (options.forgetPassword) {
            purgePassword();
        }
        boost::optional<ClientError> clientError;
        if (!disconnectRequested_) {
            clientError = std::dynamic_pointer_cast<DomainNameResolveError>(error) ? boost::optional<ClientError>(ClientError::DomainNameResolveError) : boost::optional<ClientError>(ClientError::ConnectionError);
        }
        onDisconnected(clientError);
    }
    else {
        assert(!connection_);
        assert(!sessionStream_);

        if (certificate_ && certificate_->isNull()) {
            //certificate cannot be read so do not initailise session
            onDisconnected(boost::optional<ClientError>(ClientError::ClientCertificateLoadError));
            return;
        }

        connection_ = connection;

        sessionStream_ = std::make_shared<BasicSessionStream>(ClientStreamType, connection_, getPayloadParserFactories(), getPayloadSerializers(), networkFactories->getTLSContextFactory(), networkFactories->getTimerFactory(), networkFactories->getXMLParserFactory(), options.tlsOptions);
        if (certificate_) {
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

void CoreClient::handleSessionFinished(std::shared_ptr<Error> error) {
    if (options.forgetPassword) {
        purgePassword();
    }
    resetSession();

    boost::optional<ClientError> actualError;
    if (error) {
        ClientError clientError;
        if (std::shared_ptr<ClientSession::Error> actualError = std::dynamic_pointer_cast<ClientSession::Error>(error)) {
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
                case ClientSession::Error::StreamEndError:
                    clientError = ClientError(ClientError::StreamError);
                    break;
            }
            clientError.setErrorCode(actualError->errorCode);
        }
        else if (std::shared_ptr<TLSError> actualError = std::dynamic_pointer_cast<TLSError>(error)) {
            switch(actualError->getType()) {
                case TLSError::CertificateCardRemoved:
                    clientError = ClientError(ClientError::CertificateCardRemoved);
                    break;
                case TLSError::UnknownError:
                    clientError = ClientError(ClientError::TLSError);
                    break;
            }
        }
        else if (std::shared_ptr<SessionStream::SessionStreamError> actualError = std::dynamic_pointer_cast<SessionStream::SessionStreamError>(error)) {
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
        else if (std::shared_ptr<CertificateVerificationError> verificationError = std::dynamic_pointer_cast<CertificateVerificationError>(error)) {
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

void CoreClient::sendMessage(std::shared_ptr<Message> message) {
    stanzaChannel_->sendMessage(message);
}

void CoreClient::sendPresence(std::shared_ptr<Presence> presence) {
    stanzaChannel_->sendPresence(presence);
}

void CoreClient::sendData(const std::string& data) {
    if (!sessionStream_) {
        SWIFT_LOG(warning) << "Client: Trying to send data while disconnected." << std::endl;
        return;
    }
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

bool CoreClient::isStreamEncrypted() const {
    return sessionStream_->isTLSEncrypted();
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
    for (auto f : proxyConnectionFactories) {
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
    else if (std::dynamic_pointer_cast<BOSHSessionStream>(sessionStream_)) {
        sessionStream_->close();
    }
    sessionStream_.reset();
    connection_.reset();
}

void CoreClient::forceReset() {
    if (connector_) {
        SWIFT_LOG(warning) << "Client not disconnected properly: Connector still active" << std::endl;
        resetConnector();
    }
    if (sessionStream_ || connection_) {
        SWIFT_LOG(warning) << "Client not disconnected properly: Session still active" << std::endl;
        resetSession();
    }
}

}
