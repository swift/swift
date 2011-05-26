/*
 * Copyright (c) 2011 Tobias Markmann
 * Licensed under the simplified BSD license.
 * See Documentation/Licenses/BSD-simplified.txt for more information.
 */

#include <Swiften/Parser/PayloadParsers/JingleParser.h>
#include <Swiften/Parser/PayloadParserFactory.h>
#include <Swiften/Elements/JingleContentPayload.h>
#include <Swiften/Elements/JingleFileTransferReceived.h>
#include <Swiften/Elements/JingleFileTransferHash.h>
#include <Swiften/Base/Log.h>

#include <boost/intrusive_ptr.hpp>

namespace Swift {

	JingleParser::JingleParser(PayloadParserFactoryCollection* factories) : factories(factories), level(0) {
		
	}

	void JingleParser::handleStartElement(const std::string& element, const std::string &ns, const AttributeMap& attributes) {
		if (level == 0) {
			// <jingle > tag
			JinglePayload::ref payload = getPayloadInternal();
			payload->setAction(stringToAction(attributes.getAttributeValue("action").get_value_or("")));
			payload->setInitiator(JID(attributes.getAttributeValue("initiator").get_value_or("")));
			payload->setResponder(JID(attributes.getAttributeValue("responder").get_value_or("")));
			payload->setSessionID(attributes.getAttributeValue("sid").get_value_or(""));
		}
		
		if (level == 1) {
			PayloadParserFactory* payloadParserFactory = factories->getPayloadParserFactory(element, ns, attributes);
			if (payloadParserFactory) {
				currentPayloadParser.reset(payloadParserFactory->createPayloadParser());
			}
		}
		
		if (level >= 1 && currentPayloadParser) {
			currentPayloadParser->handleStartElement(element, ns, attributes);
		}
		
		++level;
	}
	
	void JingleParser::handleEndElement(const std::string& element, const std::string &ns) {
		--level;
		if (currentPayloadParser) {
			if (level >= 1) {
				currentPayloadParser->handleEndElement(element, ns);
			}
			
			if (level == 1) {
				boost::shared_ptr<JinglePayload::Reason> reason = boost::dynamic_pointer_cast<JinglePayload::Reason>(currentPayloadParser->getPayload());
				if (reason) {
					getPayloadInternal()->setReason(*reason);
				}
				
				boost::shared_ptr<JingleContentPayload> payload = boost::dynamic_pointer_cast<JingleContentPayload>(currentPayloadParser->getPayload());
				if (payload) {
					getPayloadInternal()->addContent(payload);
				}
				
				boost::shared_ptr<JingleFileTransferReceived> received = boost::dynamic_pointer_cast<JingleFileTransferReceived>(currentPayloadParser->getPayload());
				if (received) {
					getPayloadInternal()->addPayload(received);
				}

				boost::shared_ptr<JingleFileTransferHash> hash = boost::dynamic_pointer_cast<JingleFileTransferHash>(currentPayloadParser->getPayload());
				if (hash) {
					getPayloadInternal()->addPayload(hash);
				}
			}
		}
	}
	
	void JingleParser::handleCharacterData(const std::string& data) {
		if (level > 1 && currentPayloadParser) {
			currentPayloadParser->handleCharacterData(data);
		}
	}
	
	JinglePayload::Action JingleParser::stringToAction(const std::string &str) const {
		if (str == "content-accept") {
			return JinglePayload::ContentAccept;
		} else if (str == "content-add") {
			return JinglePayload::ContentAdd;
		} else if (str == "content-modify") {
			return JinglePayload::ContentModify;
		} else if (str == "content-reject") {
			return JinglePayload::ContentReject;
		} else if (str == "content-remove") {
			return JinglePayload::ContentRemove;
		} else if (str == "description-info") {
			return JinglePayload::DescriptionInfo;
		} else if (str == "security-info") {
			return JinglePayload::SecurityInfo;
		} else if (str == "session-accept") {
			return JinglePayload::SessionAccept;
		} else if (str == "session-info") {
			return JinglePayload::SessionInfo;
		} else if (str == "session-initiate") {
			return JinglePayload::SessionInitiate;
		} else if (str == "session-terminate") {
			return JinglePayload::SessionTerminate;
		} else if (str == "transport-accept") {
			return JinglePayload::TransportAccept;
		} else if (str == "transport-info") {
			return JinglePayload::TransportInfo;
		} else if (str == "transport-reject") {
			return JinglePayload::TransportReject;
		} else if (str == "transport-replace") {
			return JinglePayload::TransportReplace;
		} else {
			return JinglePayload::UnknownAction;
		}

	}
}
