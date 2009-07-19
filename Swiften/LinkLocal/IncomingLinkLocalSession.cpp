#include "Swiften/LinkLocal/IncomingLinkLocalSession.h"

#include <boost/bind.hpp>

#include "Swiften/Elements/ProtocolHeader.h"
#include "Swiften/Network/Connection.h"
#include "Swiften/StreamStack/StreamStack.h"
#include "Swiften/StreamStack/ConnectionLayer.h"
#include "Swiften/StreamStack/XMPPLayer.h"
#include "Swiften/Elements/StreamFeatures.h"
#include "Swiften/Elements/IQ.h"
#include "Swiften/SASL/PLAINMessage.h"

namespace Swift {

IncomingLinkLocalSession::IncomingLinkLocalSession(
		const JID& localJID,
		boost::shared_ptr<Connection> connection, 
		PayloadParserFactoryCollection* payloadParserFactories, 
		PayloadSerializerCollection* payloadSerializers) :
			LinkLocalSession(
				localJID, 
				connection, 
				payloadParserFactories, 
				payloadSerializers) {
}

void IncomingLinkLocalSession::start() {
	initializeStreamStack();
}

void IncomingLinkLocalSession::handleStreamStart(const ProtocolHeader& incomingHeader) {
	remoteJID_ = JID(incomingHeader.getFrom());
	if (!remoteJID_.isValid()) {
		finishSession();
		return;
	}
	ProtocolHeader header;
	header.setFrom(getLocalJID());
	getXMPPLayer()->writeHeader(header);

	if (incomingHeader.getVersion() == "1.0") {
		getXMPPLayer()->writeElement(boost::shared_ptr<StreamFeatures>(new StreamFeatures()));
	}
	else {
		setInitialized();
	}
}

void IncomingLinkLocalSession::handleElement(boost::shared_ptr<Element> element) {
	boost::shared_ptr<Stanza> stanza = boost::dynamic_pointer_cast<Stanza>(element);
	// If we get our first stanza before streamfeatures, our session is implicitly
	// initialized
	if (stanza && !isInitialized()) {
		setInitialized();
	}
	
	if (isInitialized()) {
		if (stanza) {
			onElementReceived(stanza);
		}
		else {
			std::cerr << "Received unexpected element" << std::endl;
		}
	}
}


}
