/*
 * Copyright (c) 2011 Tobias Markmann
 * Licensed under the simplified BSD license.
 * See Documentation/Licenses/BSD-simplified.txt for more information.
 */

#pragma once

#include <Swiften/Elements/JinglePayload.h>
#include <Swiften/Parser/GenericPayloadParser.h>

namespace Swift {

class JingleReasonParser : public GenericPayloadParser<JinglePayload::Reason> {
	public:
		JingleReasonParser();

		virtual void handleStartElement(const std::string& element, const std::string&, const AttributeMap& attributes);
		virtual void handleEndElement(const std::string& element, const std::string&);
		virtual void handleCharacterData(const std::string& data);	
	private:
		JinglePayload::Reason::Type stringToReasonType(const std::string& type) const;

	private:
		int level;
		bool parseText;
		std::string text;
};

}
