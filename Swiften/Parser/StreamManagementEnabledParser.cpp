/*
 * Copyright (c) 2011 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#include <Swiften/Parser/StreamManagementEnabledParser.h>

using namespace Swift;

StreamManagementEnabledParser::StreamManagementEnabledParser() : level(TopLevel) {
}

StreamManagementEnabledParser::~StreamManagementEnabledParser() {
}

void StreamManagementEnabledParser::handleStartElement(const std::string&, const std::string&, const AttributeMap& attributes) {
	if (level == TopLevel) {
		if (attributes.getBoolAttribute("resume", false)) {
			getElementGeneric()->setResumeSupported();
		}
		getElementGeneric()->setResumeID(attributes.getAttribute("id"));
	}
	++level;
}

void StreamManagementEnabledParser::handleEndElement(const std::string&, const std::string&) {
	--level;
}
