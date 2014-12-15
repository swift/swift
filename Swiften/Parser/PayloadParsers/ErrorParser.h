/*
 * Copyright (c) 2010 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <Swiften/Elements/ErrorPayload.h>
#include <Swiften/Parser/GenericPayloadParser.h>

namespace Swift {
	class PayloadParserFactoryCollection;
	class ErrorParser : public GenericPayloadParser<ErrorPayload> {
		public:
			ErrorParser(PayloadParserFactoryCollection* factories);

			virtual void handleStartElement(const std::string& element, const std::string&, const AttributeMap& attributes);
			virtual void handleEndElement(const std::string& element, const std::string&);
			virtual void handleCharacterData(const std::string& data);

		private:
			enum Level { 
				TopLevel = 0, 
				PayloadLevel = 1
			};
			PayloadParserFactoryCollection* factories;
			int level_;
			std::string currentText_;
			boost::shared_ptr<PayloadParser> currentPayloadParser;
	};
}
