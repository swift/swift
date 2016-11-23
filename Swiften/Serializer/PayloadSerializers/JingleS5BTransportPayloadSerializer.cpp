/*
 * Copyright (c) 2011 Tobias Markmann
 * Licensed under the simplified BSD license.
 * See Documentation/Licenses/BSD-simplified.txt for more information.
 */

/*
 * Copyright (c) 2015-2016 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#include <Swiften/Serializer/PayloadSerializers/JingleS5BTransportPayloadSerializer.h>

#include <memory>

#include <boost/lexical_cast.hpp>

#include <Swiften/Base/Log.h>
#include <Swiften/Serializer/XML/XMLElement.h>
#include <Swiften/Serializer/XML/XMLNode.h>
#include <Swiften/Serializer/XML/XMLRawTextNode.h>

namespace Swift {

JingleS5BTransportPayloadSerializer::JingleS5BTransportPayloadSerializer() {
}

std::string JingleS5BTransportPayloadSerializer::serializePayload(std::shared_ptr<JingleS5BTransportPayload> payload) const {
    XMLElement payloadXML("transport", "urn:xmpp:jingle:transports:s5b:1");
    payloadXML.setAttribute("sid", payload->getSessionID());
    payloadXML.setAttribute("mode", modeToString(payload->getMode()));
    if (!payload->getDstAddr().empty()) {
        payloadXML.setAttribute("dstaddr", payload->getDstAddr());
    }

    for (const auto& candidate : payload->getCandidates()) {
        std::shared_ptr<XMLElement> candidateXML = std::make_shared<XMLElement>("candidate");
        candidateXML->setAttribute("cid", candidate.cid);
        candidateXML->setAttribute("host", candidate.hostPort.getAddress().toString());
        candidateXML->setAttribute("jid", candidate.jid.toString());
        candidateXML->setAttribute("port", boost::lexical_cast<std::string>(candidate.hostPort.getPort()));
        candidateXML->setAttribute("priority", boost::lexical_cast<std::string>(candidate.priority));
        candidateXML->setAttribute("type", typeToString(candidate.type));
        payloadXML.addNode(candidateXML);
    }

    if (payload->hasCandidateError()) {
        payloadXML.addNode(std::make_shared<XMLElement>("candidate-error"));
    }
    if (payload->hasProxyError()) {
        payloadXML.addNode(std::make_shared<XMLElement>("proxy-error"));
    }

    if (!payload->getActivated().empty()) {
        std::shared_ptr<XMLElement> activatedXML = std::make_shared<XMLElement>("activated");
        activatedXML->setAttribute("cid", payload->getActivated());
        payloadXML.addNode(activatedXML);
    }
    if (!payload->getCandidateUsed().empty()) {
        std::shared_ptr<XMLElement> candusedXML = std::make_shared<XMLElement>("candidate-used");
        candusedXML->setAttribute("cid", payload->getCandidateUsed());
        payloadXML.addNode(candusedXML);
    }

    return payloadXML.serialize();
}

std::string JingleS5BTransportPayloadSerializer::modeToString(JingleS5BTransportPayload::Mode mode) const {
    switch(mode) {
        case JingleS5BTransportPayload::TCPMode:
            return "tcp";
        case JingleS5BTransportPayload::UDPMode:
            return "udp";
    }
    assert(false);
    return "";
}

std::string JingleS5BTransportPayloadSerializer::typeToString(JingleS5BTransportPayload::Candidate::Type type) const {
    switch(type) {
        case JingleS5BTransportPayload::Candidate::AssistedType:
            return "assisted";
        case JingleS5BTransportPayload::Candidate::DirectType:
            return "direct";
        case JingleS5BTransportPayload::Candidate::ProxyType:
            return "proxy";
        case JingleS5BTransportPayload::Candidate::TunnelType:
            return "tunnel";
    }
    assert(false);
    return "";
}

}
