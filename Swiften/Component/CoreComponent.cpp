/*
 * Copyright (c) 2010 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#include <Swiften/Component/CoreComponent.h>

#include <boost/bind.hpp>
#include <iostream>

#include <Swiften/Component/ComponentSession.h>
#include <Swiften/Network/Connector.h>
#include <Swiften/Network/NetworkFactories.h>
#include <Swiften/TLS/PKCS12Certificate.h>
#include <Swiften/Session/BasicSessionStream.h>
#include <Swiften/Queries/IQRouter.h>
#include <Swiften/Base/IDGenerator.h>
#include <Swiften/Component/ComponentSessionStanzaChannel.h>

namespace Swift {

CoreComponent::CoreComponent(EventLoop* eventLoop, NetworkFactories* networkFactories, const JID& jid, const std::string& secret) : eventLoop(eventLoop), networkFactories(networkFactories), resolver_(eventLoop), jid_(jid), secret_(secret), disconnectRequested_(false) {
	stanzaChannel_ = new ComponentSessionStanzaChannel();
	stanzaChannel_->onMessageReceived.connect(boost::ref(onMessageReceived));
	stanzaChannel_->onPresenceReceived.connect(boost::ref(onPresenceReceived));
	stanzaChannel_->onAvailableChanged.connect(boost::bind(&CoreComponent::handleStanzaChannelAvailableChanged, this, _1));

	iqRouter_ = new IQRouter(stanzaChannel_);
	iqRouter_->setFrom(jid);
}

CoreComponent::~CoreComponent() {
	if (session_ || connection_) {
		std::cerr << "Warning: Component not disconnected properly" << std::endl;
	}
	delete iqRouter_;

	stanzaChannel_->onAvailableChanged.disconnect(boost::bind(&CoreComponent::handleStanzaChannelAvailableChanged, this, _1));
	stanzaChannel_->onMessageReceived.disconnect(boost::ref(onMessageReceived));
	stanzaChannel_->onPresenceReceived.disconnect(boost::ref(onPresenceReceived));
	delete stanzaChannel_;
}

void CoreComponent::connect(const std::string& host, int port) {
	assert(!connector_);
	connector_ = ComponentConnector::create(host, port, &resolver_, networkFactories->getConnectionFactory(), networkFactories->getTimerFactory());
	connector_->onConnectFinished.connect(boost::bind(&CoreComponent::handleConnectorFinished, this, _1));
	connector_->setTimeoutMilliseconds(60*1000);
	connector_->start();
}

void CoreComponent::handleConnectorFinished(boost::shared_ptr<Connection> connection) {
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
		sessionStream_ = boost::shared_ptr<BasicSessionStream>(new BasicSessionStream(ComponentStreamType, connection_, getPayloadParserFactories(), getPayloadSerializers(), NULL, networkFactories->getTimerFactory(), networkFactories->getXMLParserFactory()));
		sessionStream_->onDataRead.connect(boost::bind(&CoreComponent::handleDataRead, this, _1));
		sessionStream_->onDataWritten.connect(boost::bind(&CoreComponent::handleDataWritten, this, _1));

		session_ = ComponentSession::create(jid_, secret_, sessionStream_);
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
	assert(!session_);
	assert(!sessionStream_);
	assert(!connector_);
	disconnectRequested_ = false;
}

void CoreComponent::handleSessionFinished(boost::shared_ptr<Error> error) {
	session_->onFinished.disconnect(boost::bind(&CoreComponent::handleSessionFinished, this, _1));
	session_.reset();

	sessionStream_->onDataRead.disconnect(boost::bind(&CoreComponent::handleDataRead, this, _1));
	sessionStream_->onDataWritten.disconnect(boost::bind(&CoreComponent::handleDataWritten, this, _1));
	sessionStream_.reset();

	connection_->disconnect();
	connection_.reset();

	if (error) {
		ComponentError componentError;
		if (boost::shared_ptr<ComponentSession::Error> actualError = boost::dynamic_pointer_cast<ComponentSession::Error>(error)) {
			switch(actualError->type) {
				case ComponentSession::Error::AuthenticationFailedError:
					componentError = ComponentError(ComponentError::AuthenticationFailedError);
					break;
				case ComponentSession::Error::UnexpectedElementError:
					componentError = ComponentError(ComponentError::UnexpectedElementError);
					break;
			}
		}
		else if (boost::shared_ptr<SessionStream::SessionStreamError> actualError = boost::dynamic_pointer_cast<SessionStream::SessionStreamError>(error)) {
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

void CoreComponent::sendMessage(boost::shared_ptr<Message> message) {
	stanzaChannel_->sendMessage(message);
}

void CoreComponent::sendPresence(boost::shared_ptr<Presence> presence) {
	stanzaChannel_->sendPresence(presence);
}

}
