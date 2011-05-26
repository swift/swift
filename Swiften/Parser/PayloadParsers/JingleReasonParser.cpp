/*
 * Copyright (c) 2011 Tobias Markmann
 * Licensed under the simplified BSD license.
 * See Documentation/Licenses/BSD-simplified.txt for more information.
 */

#include "JingleReasonParser.h"

#include <Swiften/Base/Log.h>

namespace Swift {
	JingleReasonParser::JingleReasonParser() : level(0), parseText(false) {
		
	}
	
	void JingleReasonParser::handleStartElement(const std::string& element, const std::string&, const AttributeMap&) {
		if (level == 1) {
			if (element == "text") {
				parseText = true;
			} else {
				// reason type
				getPayloadInternal()->type = stringToReasonType(element);
			}
		}
		++level;
	}
	
	void JingleReasonParser::handleEndElement(const std::string& element, const std::string&) {
		--level;		
		if (element == "text") {
			parseText = false;
			getPayloadInternal()->text = text;
		}
	}
	
	void JingleReasonParser::handleCharacterData(const std::string& data) {
		if (parseText) {
			text += data;
		}
	}
	
	JinglePayload::Reason::Type JingleReasonParser::stringToReasonType(const std::string& type) const {
		if (type == "alternative-session") {
			return JinglePayload::Reason::AlternativeSession;
		} else if (type == "busy") {
			return JinglePayload::Reason::Busy;
		} else if (type == "cancel") {
			return JinglePayload::Reason::Cancel;
		} else if (type == "connectivity-error") {
			return JinglePayload::Reason::ConnectivityError;
		} else if (type == "decline") {
			return JinglePayload::Reason::Decline;
		} else if (type == "expired") {
			return JinglePayload::Reason::Expired;
		} else if (type == "failed-application") {
			return JinglePayload::Reason::FailedApplication;
		} else if (type == "failed-transport") {
			return JinglePayload::Reason::FailedTransport;
		} else if (type == "general-error") {
			return JinglePayload::Reason::GeneralError;
		} else if (type == "gone") {
			return JinglePayload::Reason::Gone;
		} else if (type == "incompatible-parameters") {
			return JinglePayload::Reason::IncompatibleParameters;
		} else if (type == "media-error") {
			return JinglePayload::Reason::MediaError;
		} else if (type == "security-error") {
			return JinglePayload::Reason::SecurityError;
		} else if (type == "success") {
			return JinglePayload::Reason::Success;
		} else if (type == "timeout") {
			return JinglePayload::Reason::Timeout;
		} else if (type == "unsupported-applications") {
			return JinglePayload::Reason::UnsupportedApplications;
		} else if (type == "unsupported-transports") {
			return JinglePayload::Reason::UnsupportedTransports;
		} else {
			return JinglePayload::Reason::UnknownType;
		}
	}
}
