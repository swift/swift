/*
 * Copyright (c) 2010-2016 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#include <Swiften/Component/CoreComponent.h>

#include <boost/bind.hpp>

#include <Swiften/Base/IDGenerator.h>
#include <Swiften/Base/Log.h>
#include <Swiften/Component/ComponentSession.h>
#include <Swiften/Component/ComponentSessionStanzaChannel.h>
#include <Swiften/Network/Connector.h>
#include <Swiften/Network/NetworkFactories.h>
#include <Swiften/Queries/IQRouter.h>
#include <Swiften/Session/BasicSessionStream.h>
#include <Swiften/TLS/PKCS12Certificate.h>
#include <Swiften/TLS/TLSOptions.h>

namespace Swift {

CoreComponent::CoreComponent(const JID& jid, const std::string& secret, NetworkFactories* networkFactories) : networkFactories(networkFactories), jid_(jid), secret_(secret), disconnectRequested_(false) {
    stanzaChannel_ = new ComponentSessionStanzaChannel();
    stanzaChannel_->onMessageReceived.connect(boost::ref(onMessageReceived));
    stanzaChannel_->onPresenceReceived.connect(boost::ref(onPresenceReceived));
    stanzaChannel_->onAvailableChanged.connect(boost::bind(&CoreComponent::handleStanzaChannelAvailableChanged, this, _1));

    iqRouter_ = new IQRouter(stanzaChannel_);
    iqRouter_->setFrom(jid);
}

CoreComponent::~CoreComponent() {
    if (session_ || connection_) {
        SWIFT_LOG(warning) << "Component not disconnected properly" << std::endl;
    }
    delete iqRouter_;

    stanzaChannel_->onAvailableChanged.disconnect(boost::bind(&CoreComponent::handleStanzaChannelAvailableChanged, this, _1));
    stanzaChannel_->onMessageReceived.disconnect(boost::ref(onMessageReceived));
    stanzaChannel_->onPresenceReceived.disconnect(boost::ref(onPresenceReceived));
    delete stanzaChannel_;
}

void CoreComponent::connect(const std::string& host, int port) {
    assert(!connector_);
    connector_ = ComponentConnector::create(host, port, networkFactories->getDomainNameResolver(), networkFactories->getConnectionFactory(), networkFactories->getTimerFactory());
    connector_->onConnectFinished.connect(boost::bind(&CoreComponent::handleConnectorFinished, this, _1));
    connector_->setTimeoutMilliseconds(60*1000);
    connector_->start();
}

void CoreComponent::handleConnectorFinished(std::shared_ptr<Connection> connection) {
    connector_->onConnectFinished.disconnect(boost::bind(&CoreComponent::handleConnectorFinished, this, _1));
    connector_.reset();
    if (!connection) {
        if (!disconnectRequested_) {
            onError(ComponentError::ConnectionError);
        }
    }
    else {
        assert(!connection_);
        connection_ = connection;

        assert(!sessionStream_);
        sessionStream_ = std::make_shared<BasicSessionStream>(ComponentStreamType, connection_, getPayloadParserFactories(), getPayloadSerializers(), nullptr, networkFactories->getTimerFactory(), networkFactories->getXMLParserFactory(), TLSOptions());
        sessionStream_->onDataRead.connect(boost::bind(&CoreComponent::handleDataRead, this, _1));
        sessionStream_->onDataWritten.connect(boost::bind(&CoreComponent::handleDataWritten, this, _1));

        session_ = ComponentSession::create(jid_, secret_, sessionStream_, networkFactories->getCryptoProvider());
        stanzaChannel_->setSession(session_);
        session_->onFinished.connect(boost::bind(&CoreComponent::handleSessionFinished, this, _1));
        session_->start();
    }
}

void CoreComponent::disconnect() {
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
    //assert(!session_); /* commenting out until we have time to refactor to be like CoreClient */
    //assert(!sessionStream_);
    //assert(!connector_);
    disconnectRequested_ = false;
}

void CoreComponent::handleSessionFinished(std::shared_ptr<Error> error) {
    session_->onFinished.disconnect(boost::bind(&CoreComponent::handleSessionFinished, this, _1));
    session_.reset();

    sessionStream_->onDataRead.disconnect(boost::bind(&CoreComponent::handleDataRead, this, _1));
    sessionStream_->onDataWritten.disconnect(boost::bind(&CoreComponent::handleDataWritten, this, _1));
    sessionStream_.reset();

    connection_->disconnect();
    connection_.reset();

    if (error) {
        ComponentError componentError;
        if (std::shared_ptr<ComponentSession::Error> actualError = std::dynamic_pointer_cast<ComponentSession::Error>(error)) {
            switch(actualError->type) {
                case ComponentSession::Error::AuthenticationFailedError:
                    componentError = ComponentError(ComponentError::AuthenticationFailedError);
                    break;
                case ComponentSession::Error::UnexpectedElementError:
                    componentError = ComponentError(ComponentError::UnexpectedElementError);
                    break;
            }
        }
        else if (std::shared_ptr<SessionStream::SessionStreamError> actualError = std::dynamic_pointer_cast<SessionStream::SessionStreamError>(error)) {
            switch(actualError->type) {
                case SessionStream::SessionStreamError::ParseError:
                    componentError = ComponentError(ComponentError::XMLError);
                    break;
                case SessionStream::SessionStreamError::TLSError:
                    assert(false);
                    componentError = ComponentError(ComponentError::UnknownError);
                    break;
                case SessionStream::SessionStreamError::InvalidTLSCertificateError:
                    assert(false);
                    componentError = ComponentError(ComponentError::UnknownError);
                    break;
                case SessionStream::SessionStreamError::ConnectionReadError:
                    componentError = ComponentError(ComponentError::ConnectionReadError);
                    break;
                case SessionStream::SessionStreamError::ConnectionWriteError:
                    componentError = ComponentError(ComponentError::ConnectionWriteError);
                    break;
            }
        }
        onError(componentError);
    }
}

void CoreComponent::handleDataRead(const SafeByteArray& data) {
    onDataRead(data);
}

void CoreComponent::handleDataWritten(const SafeByteArray& data) {
    onDataWritten(data);
}

void CoreComponent::handleStanzaChannelAvailableChanged(bool available) {
    if (available) {
        onConnected();
    }
}

void CoreComponent::sendMessage(std::shared_ptr<Message> message) {
    stanzaChannel_->sendMessage(message);
}

void CoreComponent::sendPresence(std::shared_ptr<Presence> presence) {
    stanzaChannel_->sendPresence(presence);
}

void CoreComponent::sendData(const std::string& data) {
    sessionStream_->writeData(data);
}

}
