/*
 * Copyright (c) 2011 Tobias Markmann
 * Licensed under the simplified BSD license.
 * See Documentation/Licenses/BSD-simplified.txt for more information.
 */

#pragma once

#include <Swiften/Elements/JingleS5BTransportPayload.h>
#include <Swiften/Parser/GenericPayloadParser.h>

namespace Swift {

class JingleS5BTransportMethodPayloadParser : public GenericPayloadParser<JingleS5BTransportPayload> {
	public:
		JingleS5BTransportMethodPayloadParser();

		virtual void handleStartElement(const std::string& element, const std::string&, const AttributeMap& attributes);
		virtual void handleEndElement(const std::string& element, const std::string&);
		virtual void handleCharacterData(const std::string& data);	

	private:
		JingleS5BTransportPayload::Candidate::Type stringToType(const std::string &str) const;

	private:
		int level;
};

}
