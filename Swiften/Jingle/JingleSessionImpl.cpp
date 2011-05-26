/*
 * Copyright (c) 2010 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#include <Swiften/Jingle/JingleSessionImpl.h>

#include <boost/smart_ptr/make_shared.hpp>

#include <Swiften/Parser/PayloadParsers/JingleParser.h>
#include <Swiften/Jingle/JingleContentID.h>
#include <Swiften/Elements/JingleContentPayload.h>
#include <Swiften/Queries/Request.h>
#include <Swiften/Queries/GenericRequest.h>

#include <Swiften/Base/Log.h>

#include "Swiften/Serializer/PayloadSerializers/JinglePayloadSerializer.h"
#include "Swiften/FileTransfer/JingleTransport.h"

namespace Swift {

JingleSessionImpl::JingleSessionImpl(const JID& initiator, const JID& peerJID, const std::string& id, IQRouter* router) : JingleSession(initiator, id), iqRouter(router), peerJID(peerJID) {
	SWIFT_LOG(debug) << "initiator: " << initiator << ", peerJID: " << peerJID << std::endl;
}

void JingleSessionImpl::handleIncomingAction(JinglePayload::ref action) {
	if (action->getAction() == JinglePayload::SessionTerminate) {
		onSessionTerminateReceived(action->getReason());
		return;
	}
	if (action->getAction() == JinglePayload::SessionInfo) {
		onSessionInfoReceived(action);
		return;
	}

	JingleContentPayload::ref content = action->getPayload<JingleContentPayload>();
	if (!content) {
		SWIFT_LOG(debug) << "no content payload!" << std::endl;
		return;
	}
	JingleContentID contentID(content->getName(), content->getCreator());
	JingleDescription::ref description = content->getDescriptions().empty() ? JingleDescription::ref() : content->getDescriptions()[0];
	JingleTransportPayload::ref transport = content->getTransports().empty() ? JingleTransportPayload::ref() : content->getTransports()[0];
	switch(action->getAction()) {
		case JinglePayload::SessionAccept:
			onSessionAcceptReceived(contentID, description, transport);
			return;
		case JinglePayload::TransportAccept:
			onTransportAcceptReceived(contentID, transport);
			return;
		case JinglePayload::TransportInfo:
			onTransportInfoReceived(contentID, transport);
			return;
		case JinglePayload::TransportReject:
			onTransportRejectReceived(contentID, transport);
			return;
		case JinglePayload::TransportReplace:
			onTransportReplaceReceived(contentID, transport);
			return;
		// following unused Jingle actions
		case JinglePayload::ContentAccept:
		case JinglePayload::ContentAdd:
		case JinglePayload::ContentModify:
		case JinglePayload::ContentReject:
		case JinglePayload::ContentRemove:
		case JinglePayload::DescriptionInfo:
		case JinglePayload::SecurityInfo:

		// handled elsewhere
		case JinglePayload::SessionInitiate:
		case JinglePayload::SessionInfo:
		case JinglePayload::SessionTerminate:

		case JinglePayload::UnknownAction:
			return;
	}
	std::cerr << "Unhandled Jingle action!!! ACTION: " << action->getAction() << std::endl;
}

void JingleSessionImpl::sendInitiate(const JingleContentID& id, JingleDescription::ref description, JingleTransportPayload::ref transport) {
	JinglePayload::ref payload = boost::make_shared<JinglePayload>(JinglePayload::SessionInitiate, getID());
	payload->setInitiator(getInitiator());
	JingleContentPayload::ref content = boost::make_shared<JingleContentPayload>();
	content->setCreator(id.getCreator());
	content->setName(id.getName());
	content->addDescription(description);
	content->addTransport(transport);
	payload->addPayload(content);

	sendSetRequest(payload);
}

void JingleSessionImpl::sendTerminate(JinglePayload::Reason::Type reason) {
	JinglePayload::ref payload = boost::make_shared<JinglePayload>(JinglePayload::SessionTerminate, getID());
	payload->setReason(JinglePayload::Reason(reason));
	payload->setInitiator(getInitiator());
	sendSetRequest(payload);
}

void JingleSessionImpl::sendInfo(boost::shared_ptr<Payload> info) {
	JinglePayload::ref payload = boost::make_shared<JinglePayload>(JinglePayload::SessionInfo, getID());
	payload->addPayload(info);

	sendSetRequest(payload);
}

void JingleSessionImpl::sendAccept(const JingleContentID& id, JingleDescription::ref description, JingleTransportPayload::ref transPayload) {
	JinglePayload::ref payload = createPayload();
	
	JingleContentPayload::ref content = boost::make_shared<JingleContentPayload>();
	content->setCreator(id.getCreator());
	content->setName(id.getName());
	content->addTransport(transPayload);
	content->addDescription(description);
	payload->setAction(JinglePayload::SessionAccept);
	payload->addPayload(content);
	
	// put into IQ:set and send it away
	sendSetRequest(payload);
}


void JingleSessionImpl::sendTransportAccept(const JingleContentID& id, JingleTransportPayload::ref transPayload) {
	JinglePayload::ref payload = createPayload();

	JingleContentPayload::ref content = boost::make_shared<JingleContentPayload>();
	content->setCreator(id.getCreator());
	content->setName(id.getName());
	content->addTransport(transPayload);
	payload->setAction(JinglePayload::TransportAccept);
	payload->addPayload(content);

	// put into IQ:set and send it away
	sendSetRequest(payload);
}

void JingleSessionImpl::sendTransportInfo(const JingleContentID& id, JingleTransportPayload::ref transPayload) {
	JinglePayload::ref payload = createPayload();

	JingleContentPayload::ref content = boost::make_shared<JingleContentPayload>();
	content->setCreator(id.getCreator());
	content->setName(id.getName());
	content->addTransport(transPayload);
	payload->setAction(JinglePayload::TransportInfo);
	payload->addPayload(content);

	sendSetRequest(payload);
}

void JingleSessionImpl::sendTransportReject(const JingleContentID& /* id */, JingleTransportPayload::ref /* transPayload */) {
	SWIFT_LOG(debug) << "NOT IMPLEMENTED YET!!!!" << std::endl;
}

void JingleSessionImpl::sendTransportReplace(const JingleContentID& id, JingleTransportPayload::ref transPayload) {
	JinglePayload::ref payload = createPayload();

	JingleContentPayload::ref content = boost::make_shared<JingleContentPayload>();
	content->setCreator(id.getCreator());
	content->setName(id.getName());
	content->addTransport(transPayload);
	payload->setAction(JinglePayload::TransportReplace);
	payload->addContent(content);

	sendSetRequest(payload);
}


void JingleSessionImpl::sendSetRequest(JinglePayload::ref payload) {
	boost::shared_ptr<GenericRequest<JinglePayload> > request = boost::make_shared<GenericRequest<JinglePayload> >(IQ::Set, peerJID, payload, iqRouter);
	request->send();
}


JinglePayload::ref JingleSessionImpl::createPayload() const {
	JinglePayload::ref payload = boost::make_shared<JinglePayload>();
	payload->setSessionID(getID());
	payload->setInitiator(getInitiator());
	return payload;
}



}
