/*
 * Copyright (c) 2011 Tobias Markmann
 * Licensed under the simplified BSD license.
 * See Documentation/Licenses/BSD-simplified.txt for more information.
 */

#pragma once

#include <Swiften/Elements/JingleContentPayload.h>
#include <Swiften/Parser/GenericPayloadParser.h>

namespace Swift {

class PayloadParserFactoryCollection;

class JingleContentPayloadParser : public GenericPayloadParser<JingleContentPayload> {
	public:
		JingleContentPayloadParser(PayloadParserFactoryCollection* factories);

		virtual void handleStartElement(const std::string& element, const std::string&, const AttributeMap& attributes);
		virtual void handleEndElement(const std::string& element, const std::string&);
		virtual void handleCharacterData(const std::string& data);	

	private:
		PayloadParserFactoryCollection* factories;
		int level;
		boost::shared_ptr<PayloadParser> currentPayloadParser;
};

}
