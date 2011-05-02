/*
 * Copyright (c) 2010 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#pragma once


#include <Swiften/Parser/XMLParserClient.h>
#include <Swiften/Parser/PlatformXMLParserFactory.h>
#include <Swiften/Parser/XMLParser.h>

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

			bool parse(const std::string& data) {
				return xmlParser_->parse(data);
			}

			virtual void handleStartElement(const std::string& element, const std::string& ns, const AttributeMap& attributes) {
				parser_->handleStartElement(element, ns, attributes);
			}

			virtual void handleEndElement(const std::string& element, const std::string& ns) {
				parser_->handleEndElement(element, ns);
			}

			virtual void handleCharacterData(const std::string& data) {
				parser_->handleCharacterData(data);
			}

		private:
			XMLParser* xmlParser_;
			ParserType* parser_;
	};
}
