/*
 * Copyright (c) 2010 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#include <Swiften/Parser/StreamErrorParser.h>

namespace Swift {

StreamErrorParser::StreamErrorParser() : level(TopLevel) {
}

void StreamErrorParser::handleStartElement(const std::string&, const std::string&, const AttributeMap&) {
	++level;
}

void StreamErrorParser::handleEndElement(const std::string& element, const std::string& ns) {
	--level;
	if (level == ElementLevel && ns == "urn:ietf:params:xml:ns:xmpp-streams") {
		if (element == "text") {
			getElementGeneric()->setText(currentText);
		}
		else if (element == "bad-format") {
			getElementGeneric()->setType(StreamError::BadFormat);
		}
		else if(element == "bad-namespace-prefix") {
			getElementGeneric()->setType(StreamError::BadNamespacePrefix);
		}
		else if(element == "conflict") {
			getElementGeneric()->setType(StreamError::Conflict);
		}
		else if(element == "connection-timeout") {
			getElementGeneric()->setType(StreamError::ConnectionTimeout);
		}
		else if(element == "host-gone") {
			getElementGeneric()->setType(StreamError::HostGone);
		}
		else if(element == "host-unknown") {
			getElementGeneric()->setType(StreamError::HostUnknown);
		}
		else if(element == "improper-addressing") {
			getElementGeneric()->setType(StreamError::ImproperAddressing);
		}
		else if(element == "internal-server-error") {
			getElementGeneric()->setType(StreamError::InternalServerError);
		}
		else if(element == "invalid-from") {
			getElementGeneric()->setType(StreamError::InvalidFrom);
		}
		else if(element == "invalid-id") {
			getElementGeneric()->setType(StreamError::InvalidID);
		}
		else if(element == "invalid-namespace") {
			getElementGeneric()->setType(StreamError::InvalidNamespace);
		}
		else if(element == "invalid-xml") {
			getElementGeneric()->setType(StreamError::InvalidXML);
		}
		else if(element == "not-authorized") {
			getElementGeneric()->setType(StreamError::NotAuthorized);
		}
		else if(element == "not-well-formed") {
			getElementGeneric()->setType(StreamError::NotWellFormed);
		}
		else if(element == "policy-violation") {
			getElementGeneric()->setType(StreamError::PolicyViolation);
		}
		else if(element == "remote-connection-failed") {
			getElementGeneric()->setType(StreamError::RemoteConnectionFailed);
		}
		else if(element == "reset") {
			getElementGeneric()->setType(StreamError::Reset);
		}
		else if(element == "resource-constraint") {
			getElementGeneric()->setType(StreamError::ResourceConstraint);
		}
		else if(element == "restricted-xml") {
			getElementGeneric()->setType(StreamError::RestrictedXML);
		}
		else if(element == "see-other-host") {
			getElementGeneric()->setType(StreamError::SeeOtherHost);
		}
		else if(element == "system-shutdown") {
			getElementGeneric()->setType(StreamError::SystemShutdown);
		}
		else if(element == "undefined-condition") {
			getElementGeneric()->setType(StreamError::UndefinedCondition);
		}
		else if(element == "unsupported-encoding") {
			getElementGeneric()->setType(StreamError::UnsupportedEncoding);
		}
		else if(element == "unsupported-stanza-type") {
			getElementGeneric()->setType(StreamError::UnsupportedStanzaType);
		}
		else if(element == "unsupported-version") {
			getElementGeneric()->setType(StreamError::UnsupportedVersion);
		}
		else {
			getElementGeneric()->setType(StreamError::UndefinedCondition);
		}
	}
}

void StreamErrorParser::handleCharacterData(const std::string& data) {
	currentText += data;
}

}
