/*
 * Copyright (c) 2011 Tobias Markmann
 * Licensed under the simplified BSD license.
 * See Documentation/Licenses/BSD-simplified.txt for more information.
 */

#include <boost/lexical_cast.hpp>
#include <boost/optional.hpp>

#include "JingleS5BTransportMethodPayloadParser.h"

#include <Swiften/Base/Log.h>

namespace Swift {
	JingleS5BTransportMethodPayloadParser::JingleS5BTransportMethodPayloadParser() : level(0) {
		
	}
	
	void JingleS5BTransportMethodPayloadParser::handleStartElement(const std::string& element, const std::string&, const AttributeMap& attributes) {
		if (level == 0) {
			getPayloadInternal()->setSessionID(attributes.getAttributeValue("sid").get_value_or(""));
			std::string mode = attributes.getAttributeValue("mode").get_value_or("tcp");
			if (mode == "tcp") {
				getPayloadInternal()->setMode(JingleS5BTransportPayload::TCPMode);
			} else if(mode == "udp") {
				getPayloadInternal()->setMode(JingleS5BTransportPayload::UDPMode);
			} else {
				std::cerr << "Unknown S5B mode; falling back to defaul!" << std::endl;
				getPayloadInternal()->setMode(JingleS5BTransportPayload::TCPMode);
			}
		} else if (level == 1) {
			if (element == "candidate") {
				JingleS5BTransportPayload::Candidate candidate;
				candidate.cid = attributes.getAttributeValue("cid").get_value_or("");

				int port = -1;
				try {
					port = boost::lexical_cast<int>(attributes.getAttributeValue("port").get_value_or("-1"));
				} catch(boost::bad_lexical_cast &) { }
				candidate.hostPort = HostAddressPort(HostAddress(attributes.getAttributeValue("host").get_value_or("")), port);
				candidate.jid = JID(attributes.getAttributeValue("jid").get_value_or(""));
				int priority = -1;
				try {
					priority = boost::lexical_cast<int>(attributes.getAttributeValue("priority").get_value_or("-1"));
				} catch(boost::bad_lexical_cast &) { }
				candidate.priority = priority;
				candidate.type = stringToType(attributes.getAttributeValue("type").get_value_or("direct"));

				getPayloadInternal()->addCandidate(candidate);
			} else if (element == "candidate-used") {
				getPayloadInternal()->setCandidateUsed(attributes.getAttributeValue("cid").get_value_or(""));
			} else if (element == "candidate-error") {
				getPayloadInternal()->setCandidateError(true);
			} else if (element == "activated") {
				getPayloadInternal()->setActivated(attributes.getAttributeValue("cid").get_value_or(""));
			} else if (element == "proxy-error") {
				getPayloadInternal()->setProxyError(true);
			}
		}

		++level;
	}
	
	void JingleS5BTransportMethodPayloadParser::handleEndElement(const std::string&, const std::string&) {
		--level;
		

	}
	
	void JingleS5BTransportMethodPayloadParser::handleCharacterData(const std::string&) {

	}

	JingleS5BTransportPayload::Candidate::Type JingleS5BTransportMethodPayloadParser::stringToType(const std::string &str) const {
		if (str == "direct") {
			return JingleS5BTransportPayload::Candidate::DirectType;
		} else if (str == "assisted") {
			return JingleS5BTransportPayload::Candidate::AssistedType;
		} else if (str == "tunnel") {
			return JingleS5BTransportPayload::Candidate::TunnelType;
		} else if (str == "proxy") {
			return JingleS5BTransportPayload::Candidate::ProxyType;
		} else {
			std::cerr << "Unknown candidate type; falling back to default!" << std::endl;
			return JingleS5BTransportPayload::Candidate::DirectType;
		}
	}
}
