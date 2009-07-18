// TODO: Send back errors if we can't make a connection

#include "Swiften/LinkLocal/OutgoingLinkLocalSession.h"

#include <boost/bind.hpp>

#include "Swiften/Elements/ProtocolHeader.h"
#include "Swiften/Network/Connection.h"
#include "Swiften/Network/ConnectionFactory.h"
#include "Swiften/Network/HostAddress.h"
#include "Swiften/Network/HostAddressPort.h"
#include "Swiften/StreamStack/StreamStack.h"
#include "Swiften/LinkLocal/DNSSDService.h"
#include "Swiften/StreamStack/ConnectionLayer.h"
#include "Swiften/StreamStack/XMPPLayer.h"
#include "Swiften/Elements/StreamFeatures.h"
#include "Swiften/Elements/IQ.h"
#include "Swiften/SASL/PLAINMessage.h"

namespace Swift {

OutgoingLinkLocalSession::OutgoingLinkLocalSession(
		const JID& localJID,
		const JID& remoteJID,
		const String& hostname,
		int port,
		boost::shared_ptr<DNSSDService> resolver,
		PayloadParserFactoryCollection* payloadParserFactories, 
		PayloadSerializerCollection* payloadSerializers,
		ConnectionFactory* connectionFactory) :
			LinkLocalSession(
				localJID, 
				boost::shared_ptr<Connection>(), 
				payloadParserFactories, 
				payloadSerializers),
			resolving_(false),
			remoteJID_(remoteJID),
			hostname_(hostname),
			port_(port),
			resolver_(resolver),
			connectionFactory_(connectionFactory) {
}

void OutgoingLinkLocalSession::start() {
	resolving_ = true;
	resolver_->onHostnameResolved.connect(boost::bind(&OutgoingLinkLocalSession::handleHostnameResolved, boost::dynamic_pointer_cast<OutgoingLinkLocalSession>(shared_from_this()), _1, _2));
	resolver_->resolveHostname(hostname_);
}

void OutgoingLinkLocalSession::handleHostnameResolved(const String& hostname, const boost::optional<HostAddress>& address) {
	if (resolving_) {
		if (hostname == hostname_) {
			resolving_ = false;
			if (address) {
				boost::shared_ptr<Connection> connection = connectionFactory_->createConnection();
				setConnection(connection);
				initializeStreamStack();
				connection->onConnected.connect(boost::bind(&OutgoingLinkLocalSession::handleConnected, boost::dynamic_pointer_cast<OutgoingLinkLocalSession>(shared_from_this())));
				connection->connect(HostAddressPort(*address, port_));
			}
			else {
				onSessionFinished(boost::optional<Error>(UnknownError));
			}
		}
	}
}

void OutgoingLinkLocalSession::handleConnected() {
	ProtocolHeader header;
	header.setFrom(getLocalJID());
	getXMPPLayer()->writeHeader(header);
}

void OutgoingLinkLocalSession::handleStreamStart(const ProtocolHeader&) {
	foreach(const boost::shared_ptr<Stanza>& stanza, queuedStanzas_) {
		LinkLocalSession::sendStanza(stanza);
	}
	queuedStanzas_.clear();
	setInitialized();
}

void OutgoingLinkLocalSession::handleElement(boost::shared_ptr<Element> element) {
	if (isInitialized()) {
		boost::shared_ptr<Stanza> stanza = boost::dynamic_pointer_cast<Stanza>(element);
		if (stanza) {
			onStanzaReceived(stanza);
		}
	}
}

void OutgoingLinkLocalSession::sendStanza(boost::shared_ptr<Stanza> stanza) {
	if (isInitialized()) {
		LinkLocalSession::sendStanza(stanza);
	}
	else {
		queuedStanzas_.push_back(stanza);
	}
}


}
