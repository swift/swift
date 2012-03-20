/*
 * Copyright (c) 2011 Tobias Markmann
 * Licensed under the simplified BSD license.
 * See Documentation/Licenses/BSD-simplified.txt for more information.
 */

#include <Swiften/Serializer/PayloadSerializers/JinglePayloadSerializer.h>

#include <boost/shared_ptr.hpp>
#include <boost/smart_ptr/make_shared.hpp>
#include <boost/smart_ptr/intrusive_ptr.hpp>

#include <Swiften/Base/foreach.h>
#include <Swiften/Serializer/XML/XMLNode.h>
#include <Swiften/Serializer/XML/XMLElement.h>
#include <Swiften/Serializer/XML/XMLRawTextNode.h>
#include <Swiften/Serializer/PayloadSerializers/JingleContentPayloadSerializer.h>
#include <Swiften/Serializer/PayloadSerializers/JingleFileTransferHashSerializer.h>
#include <Swiften/Serializer/PayloadSerializers/JingleFileTransferReceivedSerializer.h>

#include <Swiften/Serializer/PayloadSerializerCollection.h>

#include <Swiften/Elements/JinglePayload.h>
#include <Swiften/Elements/JingleContentPayload.h>
#include <Swiften/Elements/JingleIBBTransportPayload.h>
#include <Swiften/Elements/JingleFileTransferDescription.h>
#include <Swiften/Elements/JingleFileTransferHash.h>
#include <Swiften/Elements/JingleFileTransferReceived.h>

namespace Swift {

JinglePayloadSerializer::JinglePayloadSerializer(PayloadSerializerCollection* serializers) : serializers(serializers) {
}

std::string JinglePayloadSerializer::serializePayload(boost::shared_ptr<JinglePayload> payload) const {
	XMLElement jinglePayload("jingle", "urn:xmpp:jingle:1");
	jinglePayload.setAttribute("action", actionToString(payload->getAction()));
	jinglePayload.setAttribute("initiator", payload->getInitiator());
	jinglePayload.setAttribute("sid", payload->getSessionID());

	std::vector<boost::shared_ptr<Payload> > payloads = payload->getPayloads();
	if (!payloads.empty()) {
		foreach(boost::shared_ptr<Payload> subPayload, payloads) {
			PayloadSerializer* serializer = serializers->getPayloadSerializer(subPayload);
			if (serializer) {
				jinglePayload.addNode(boost::make_shared<XMLRawTextNode>(serializer->serialize(subPayload)));
			}
		}
	}
	
	if (payload->getReason().is_initialized()) {
		boost::shared_ptr<XMLElement> reason = boost::make_shared<XMLElement>("reason");
		reason->addNode(boost::make_shared<XMLElement>(reasonTypeToString(payload->getReason()->type)));
		if (!payload->getReason()->text.empty()) {
			reason->addNode(boost::make_shared<XMLElement>("desc", "", payload->getReason()->text));
		}
		jinglePayload.addNode(reason);
	}
	
	return jinglePayload.serialize();
}

std::string JinglePayloadSerializer::actionToString(JinglePayload::Action action) const {
	switch(action) {
		case JinglePayload::ContentAccept:
			return "content-accept";
		case JinglePayload::ContentAdd:
			return "content-add";
		case JinglePayload::ContentModify:
			return "content-modify";
		case JinglePayload::ContentReject:
			return "content-reject";
		case JinglePayload::ContentRemove:
			return "content-remove";
		case JinglePayload::DescriptionInfo:
			return "description-info";
		case JinglePayload::SecurityInfo:
			return "security-info";
		case JinglePayload::SessionAccept:
			return "session-accept";
		case JinglePayload::SessionInfo:
			return "session-info";
		case JinglePayload::SessionInitiate:
			return "session-initiate";
		case JinglePayload::SessionTerminate:
			return "session-terminate";
		case JinglePayload::TransportAccept:
			return "transport-accept";
		case JinglePayload::TransportInfo:
			return "transport-info";
		case JinglePayload::TransportReject:
			return "transport-reject";
		case JinglePayload::TransportReplace:
			return "transport-replace";
		case JinglePayload::UnknownAction:
			std::cerr << "Serializing unknown action value." << std::endl;
			return "";
	}
	assert(false);
	return "";
}

std::string JinglePayloadSerializer::reasonTypeToString(JinglePayload::Reason::Type type) const {
	switch(type) {
		case JinglePayload::Reason::UnknownType:
			std::cerr << "Unknown jingle reason type!" << std::endl;
			return "";
		case JinglePayload::Reason::AlternativeSession:
			return "alternative-session";
		case JinglePayload::Reason::Busy:
			return "busy";
		case JinglePayload::Reason::Cancel:
			return "cancel";
		case JinglePayload::Reason::ConnectivityError:
			return "connectivity-error";
		case JinglePayload::Reason::Decline:
			return "decline";
		case JinglePayload::Reason::Expired:
			return "expired";
		case JinglePayload::Reason::FailedApplication:
			return "failed-application";
		case JinglePayload::Reason::FailedTransport:
			return "failed-transport";
		case JinglePayload::Reason::GeneralError:
			return "general-error";
		case JinglePayload::Reason::Gone:
			return "gone";
		case JinglePayload::Reason::IncompatibleParameters:
			return "incompatible-parameters";
		case JinglePayload::Reason::MediaError:
			return "media-error";
		case JinglePayload::Reason::SecurityError:
			return "security-error";
		case JinglePayload::Reason::Success:
			return "success";
		case JinglePayload::Reason::Timeout:
			return "timeout";
		case JinglePayload::Reason::UnsupportedApplications:
			return "unsupported-applications";
		case JinglePayload::Reason::UnsupportedTransports:
			return "unsupported-transports";
	}
	assert(false);
	return "";
}

}
