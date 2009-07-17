#include "Swiften/Server/ServerFromClientSession.h"

#include <boost/bind.hpp>

#include "Swiften/Server/UserRegistry.h"
#include "Swiften/Network/Connection.h"
#include "Swiften/StreamStack/StreamStack.h"
#include "Swiften/StreamStack/ConnectionLayer.h"
#include "Swiften/StreamStack/XMPPLayer.h"
#include "Swiften/Elements/StreamFeatures.h"
#include "Swiften/Elements/ResourceBind.h"
#include "Swiften/Elements/StartSession.h"
#include "Swiften/Elements/IQ.h"
#include "Swiften/Elements/AuthSuccess.h"
#include "Swiften/Elements/AuthFailure.h"
#include "Swiften/Elements/AuthRequest.h"
#include "Swiften/SASL/PLAINMessage.h"

namespace Swift {

ServerFromClientSession::ServerFromClientSession(
		const String& id,
		boost::shared_ptr<Connection> connection, 
		PayloadParserFactoryCollection* payloadParserFactories, 
		PayloadSerializerCollection* payloadSerializers,
		UserRegistry* userRegistry) : 
			id_(id),
			connection_(connection), 
			payloadParserFactories_(payloadParserFactories), 
			payloadSerializers_(payloadSerializers),
			userRegistry_(userRegistry),
			authenticated_(false),
			initialized_(false) {
	xmppLayer_ = boost::shared_ptr<XMPPLayer>(new XMPPLayer(payloadParserFactories_, payloadSerializers_));
	connectionLayer_ = boost::shared_ptr<ConnectionLayer>(new ConnectionLayer(connection_));
	streamStack_ = new StreamStack(xmppLayer_, connectionLayer_);
}

ServerFromClientSession::~ServerFromClientSession() {
	delete streamStack_;
}

void ServerFromClientSession::start() {
	xmppLayer_->onStreamStart.connect(
			boost::bind(&ServerFromClientSession::handleStreamStart, this, _2));
	xmppLayer_->onElement.connect(
			boost::bind(&ServerFromClientSession::handleElement, this, _1));
	//xmppLayer_->onError.connect(
	//		boost::bind(&ServerFromClientSession::setError, this, XMLError));
	xmppLayer_->onDataRead.connect(
			boost::bind(boost::ref(onDataRead), _1));
	xmppLayer_->onWriteData.connect(
			boost::bind(boost::ref(onDataWritten), _1));
	connection_->onDisconnected.connect(boost::bind(&ServerFromClientSession::handleDisconnected, shared_from_this(), _1));
}

void ServerFromClientSession::handleElement(boost::shared_ptr<Element> element) {
	if (initialized_) {
		if (boost::shared_ptr<Stanza> stanza = boost::dynamic_pointer_cast<Stanza>(element)) {
			onStanzaReceived(stanza);
		}
		else {
			std::cerr << "Received unexpected element" << std::endl;
		}
	}
	else {
		if (AuthRequest* authRequest = dynamic_cast<AuthRequest*>(element.get())) {
			if (authRequest->getMechanism() != "PLAIN") {
				xmppLayer_->writeElement(boost::shared_ptr<AuthFailure>(new AuthFailure));
				onSessionFinished();
			}
			else {
				PLAINMessage plainMessage(authRequest->getMessage());
				if (userRegistry_->isValidUserPassword(JID(plainMessage.getAuthenticationID(), domain_.getDomain()), plainMessage.getPassword())) {
					xmppLayer_->writeElement(boost::shared_ptr<AuthSuccess>(new AuthSuccess()));
					user_ = plainMessage.getAuthenticationID();
					authenticated_ = true;
					xmppLayer_->resetParser();
				}
				else {
					xmppLayer_->writeElement(boost::shared_ptr<AuthFailure>(new AuthFailure));
					onSessionFinished();
				}
			}
		}
		else if (IQ* iq = dynamic_cast<IQ*>(element.get())) {
			if (boost::shared_ptr<ResourceBind> resourceBind = iq->getPayload<ResourceBind>()) {
				jid_ = JID(user_, domain_.getDomain(), resourceBind->getResource());
				boost::shared_ptr<ResourceBind> resultResourceBind(new ResourceBind());
				resultResourceBind->setJID(jid_);
				xmppLayer_->writeElement(IQ::createResult(JID(), iq->getID(), resultResourceBind));
			}
			else if (iq->getPayload<StartSession>()) {
				initialized_ = true;
				xmppLayer_->writeElement(IQ::createResult(jid_, iq->getID()));
				onSessionStarted();
			}
		}
	}
}

void ServerFromClientSession::handleStreamStart(const String& domain) {
	domain_ = JID("", domain);
	xmppLayer_->writeHeader(domain, id_);

	boost::shared_ptr<StreamFeatures> features(new StreamFeatures());
	if (!authenticated_) {
		features->addAuthenticationMechanism("PLAIN");
	}
	else {
		features->setHasResourceBind();
		features->setHasSession();
	}
	xmppLayer_->writeElement(features);
}

void ServerFromClientSession::sendStanza(boost::shared_ptr<Stanza> stanza) {
	xmppLayer_->writeElement(stanza);
}

void ServerFromClientSession::handleDisconnected(const boost::optional<Connection::Error>&) {
	onSessionFinished();
}


}
