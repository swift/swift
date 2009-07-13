#include "Swiften/Server/ServerFromClientSession.h"

#include <boost/bind.hpp>

#include "Swiften/Server/UserRegistry.h"
#include "Swiften/Network/IncomingConnection.h"
#include "Swiften/StreamStack/StreamStack.h"
#include "Swiften/StreamStack/IncomingConnectionLayer.h"
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
		boost::shared_ptr<IncomingConnection> connection, 
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
	xmppLayer_ = new XMPPLayer(payloadParserFactories_, payloadSerializers_);
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
	connectionLayer_ = new IncomingConnectionLayer(connection_.get());
	streamStack_ = new StreamStack(xmppLayer_, connectionLayer_);
}

ServerFromClientSession::~ServerFromClientSession() {
	delete streamStack_;
	delete connectionLayer_;
	delete xmppLayer_;
}

void ServerFromClientSession::handleElement(boost::shared_ptr<Element> element) {
	if (initialized_) {
		onElementReceived(element);
	}
	else {
		if (AuthRequest* authRequest = dynamic_cast<AuthRequest*>(element.get())) {
			if (authRequest->getMechanism() != "PLAIN") {
				xmppLayer_->writeElement(boost::shared_ptr<AuthFailure>(new AuthFailure));
				onSessionFinished();
			}
			else {
				PLAINMessage plainMessage(authRequest->getMessage());
				if (userRegistry_->isValidUserPassword(JID(plainMessage.getAuthenticationID(), domain_), plainMessage.getPassword())) {
					xmppLayer_->writeElement(boost::shared_ptr<AuthSuccess>(new AuthSuccess()));
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
				jid_ = JID(user_, domain_, resourceBind->getResource());
				boost::shared_ptr<ResourceBind> resultResourceBind(new ResourceBind());
				resultResourceBind->setJID(jid_);
				xmppLayer_->writeElement(IQ::createResult(JID(), iq->getID(), resultResourceBind));
			}
			else if (iq->getPayload<StartSession>()) {
				initialized_ = true;
				xmppLayer_->writeElement(IQ::createResult(jid_, iq->getID()));
			}
		}
	}
}

void ServerFromClientSession::handleStreamStart(const String& domain) {
	domain_ = domain;
	xmppLayer_->writeHeader(domain_, id_);

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

}
