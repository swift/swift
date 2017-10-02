/*
 * Copyright (c) 2010-2016 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#include <Swiften/Jingle/JingleSessionImpl.h>

#include <algorithm>
#include <memory>

#include <boost/bind.hpp>

#include <Swiften/Base/Log.h>
#include <Swiften/Elements/JingleContentPayload.h>
#include <Swiften/Jingle/JingleContentID.h>
#include <Swiften/Jingle/JingleSessionListener.h>
#include <Swiften/Parser/PayloadParsers/JingleParser.h>
#include <Swiften/Queries/GenericRequest.h>
#include <Swiften/Queries/Request.h>
#include <Swiften/Serializer/PayloadSerializers/JinglePayloadSerializer.h>

namespace Swift {

JingleSessionImpl::JingleSessionImpl(const JID& initiator, const JID& peerJID, const std::string& id, IQRouter* router) : JingleSession(initiator, id), iqRouter(router), peerJID(peerJID) {
    SWIFT_LOG(debug) << "initiator: " << initiator << ", peerJID: " << peerJID << std::endl;
}

void JingleSessionImpl::handleIncomingAction(JinglePayload::ref action) {
    if (action->getAction() == JinglePayload::SessionTerminate) {
        notifyListeners(&JingleSessionListener::handleSessionTerminateReceived, action->getReason());
        return;
    }
    if (action->getAction() == JinglePayload::SessionInfo) {
        notifyListeners(&JingleSessionListener::handleSessionInfoReceived, action);
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
            notifyListeners(&JingleSessionListener::handleSessionAcceptReceived, contentID, description, transport);
            return;
        case JinglePayload::TransportAccept:
            notifyListeners(&JingleSessionListener::handleTransportAcceptReceived, contentID, transport);
            return;
        case JinglePayload::TransportInfo:
            notifyListeners(&JingleSessionListener::handleTransportInfoReceived, contentID, transport);
            return;
        case JinglePayload::TransportReject:
            notifyListeners(&JingleSessionListener::handleTransportRejectReceived, contentID, transport);
            return;
        case JinglePayload::TransportReplace:
            notifyListeners(&JingleSessionListener::handleTransportReplaceReceived, contentID, transport);
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
    assert(false);
}

void JingleSessionImpl::sendInitiate(const JingleContentID& id, JingleDescription::ref description, JingleTransportPayload::ref transport) {
    JinglePayload::ref payload = std::make_shared<JinglePayload>(JinglePayload::SessionInitiate, getID());
    payload->setInitiator(getInitiator());
    JingleContentPayload::ref content = std::make_shared<JingleContentPayload>();
    content->setCreator(id.getCreator());
    content->setName(id.getName());
    content->addDescription(description);
    content->addTransport(transport);
    payload->addPayload(content);

    sendSetRequest(payload);
}

void JingleSessionImpl::sendTerminate(JinglePayload::Reason::Type reason) {
    JinglePayload::ref payload = std::make_shared<JinglePayload>(JinglePayload::SessionTerminate, getID());
    payload->setReason(JinglePayload::Reason(reason));
    payload->setInitiator(getInitiator());
    sendSetRequest(payload);
}

void JingleSessionImpl::sendInfo(std::shared_ptr<Payload> info) {
    JinglePayload::ref payload = std::make_shared<JinglePayload>(JinglePayload::SessionInfo, getID());
    payload->addPayload(info);

    sendSetRequest(payload);
}

void JingleSessionImpl::sendAccept(const JingleContentID& id, JingleDescription::ref description, JingleTransportPayload::ref transPayload) {
    JinglePayload::ref payload = createPayload();

    JingleContentPayload::ref content = std::make_shared<JingleContentPayload>();
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

    JingleContentPayload::ref content = std::make_shared<JingleContentPayload>();
    content->setCreator(id.getCreator());
    content->setName(id.getName());
    content->addTransport(transPayload);
    payload->setAction(JinglePayload::TransportAccept);
    payload->addPayload(content);

    // put into IQ:set and send it away
    sendSetRequest(payload);
}

std::string JingleSessionImpl::sendTransportInfo(const JingleContentID& id, JingleTransportPayload::ref transPayload) {
    JinglePayload::ref payload = createPayload();

    JingleContentPayload::ref content = std::make_shared<JingleContentPayload>();
    content->setCreator(id.getCreator());
    content->setName(id.getName());
    content->addTransport(transPayload);
    payload->setAction(JinglePayload::TransportInfo);
    payload->addPayload(content);

    return sendSetRequest(payload);
}

void JingleSessionImpl::sendTransportReject(const JingleContentID& id, JingleTransportPayload::ref transPayload) {
    JinglePayload::ref payload = createPayload();

    JingleContentPayload::ref content = std::make_shared<JingleContentPayload>();
    content->setCreator(id.getCreator());
    content->setName(id.getName());
    content->addTransport(transPayload);
    payload->setAction(JinglePayload::TransportReject);
    payload->addPayload(content);

    sendSetRequest(payload);
}

void JingleSessionImpl::sendTransportReplace(const JingleContentID& id, JingleTransportPayload::ref transPayload) {
    JinglePayload::ref payload = createPayload();

    JingleContentPayload::ref content = std::make_shared<JingleContentPayload>();
    content->setCreator(id.getCreator());
    content->setName(id.getName());
    content->addTransport(transPayload);
    payload->setAction(JinglePayload::TransportReplace);
    payload->addContent(content);

    sendSetRequest(payload);
}


std::string JingleSessionImpl::sendSetRequest(JinglePayload::ref payload) {
    std::shared_ptr<GenericRequest<JinglePayload> > request = std::make_shared<GenericRequest<JinglePayload> >(
            IQ::Set, peerJID, payload, iqRouter);
    pendingRequests.insert(std::make_pair(
        request,
        request->onResponse.connect(boost::bind(&JingleSessionImpl::handleRequestResponse, this, request))));
    return request->send();
}


JinglePayload::ref JingleSessionImpl::createPayload() const {
    JinglePayload::ref payload = std::make_shared<JinglePayload>();
    payload->setSessionID(getID());
    payload->setInitiator(getInitiator());
    return payload;
}

void JingleSessionImpl::handleRequestResponse(RequestRef request) {
    RequestsMap::iterator i = pendingRequests.find(request);
    assert(i != pendingRequests.end());
    if (i->first->getPayloadGeneric()->getAction() == JinglePayload::TransportInfo) {
        notifyListeners(&JingleSessionListener::handleTransportInfoAcknowledged, i->first->getID());
    }
    i->second.disconnect();
    pendingRequests.erase(i);
}


}
