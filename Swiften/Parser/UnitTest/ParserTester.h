/*
 * Copyright (c) 2010 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#ifndef SWIFTEN_ParserTester_H
#define SWIFTEN_ParserTester_H

#include "Swiften/Parser/XMLParserClient.h"
#include "Swiften/Parser/PlatformXMLParserFactory.h"
#include "Swiften/Parser/XMLParser.h"

namespace Swift {
	class XMLParser;

	template<typename ParserType>
	class ParserTester : public XMLParserClient {
		public:
			ParserTester(ParserType* parser) : parser_(parser) {
				xmlParser_ = PlatformXMLParserFactory().createXMLParser(this);
			}

			~ParserTester() {
				delete xmlParser_;
			}

			bool parse(const String& data) {
				return xmlParser_->parse(data);
			}

			virtual void handleStartElement(const String& element, const String& ns, const AttributeMap& attributes) {
				parser_->handleStartElement(element, ns, attributes);
			}

			virtual void handleEndElement(const String& element, const String& ns) {
				parser_->handleEndElement(element, ns);
			}

			virtual void handleCharacterData(const String& data) {
				parser_->handleCharacterData(data);
			}

		private:
			XMLParser* xmlParser_;
			ParserType* parser_;
	};
}

#endif
