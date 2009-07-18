#include "Swiften/LinkLocal/OutgoingLinkLocalSession.h"

#include <boost/bind.hpp>

#include "Swiften/Elements/ProtocolHeader.h"
#include "Swiften/Network/Connection.h"
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
			resolver_(resolver),
			connectionFactory_(connectionFactory) {
}

void OutgoingLinkLocalSession::start() {
	resolving_ = true;
	//resolver_->onHostnameResolved.connect(boost::bind(&OutgoingLinkLocalSession::handleHostnameResolved, this, _1, _2));
}

#if 0
void OutgoingLinkLocalSession::handleHostnameResolved(const String& hostname, const HostAddress&) {
	if (resolving_) {
		if (hostname == hostname_) {
			boost::shared_ptr<Connection> connection = connectionFactory_->createConnection();
			connection->onConnected.connect(boost::bind(&Session::handleConnected, shared_from_this()));
			connection->onDisconnected.connect(boost::bind(&Session::handleDisconnected, shared_from_this(), _1));
			connection_->connect(jid_.getDomain());
			resolving_ = false;
			boost::
		}
	}
}
#endif

void OutgoingLinkLocalSession::handleStreamStart(const ProtocolHeader&) {
	ProtocolHeader header;
	header.setFrom(getLocalJID());
	getXMPPLayer()->writeHeader(header);
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


}
