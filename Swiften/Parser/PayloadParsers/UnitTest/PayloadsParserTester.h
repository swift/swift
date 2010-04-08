/*
 * Copyright (c) 2010 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#pragma once

#include <cppunit/extensions/HelperMacros.h>

#include "Swiften/Parser/PayloadParsers/FullPayloadParserFactoryCollection.h"
#include "Swiften/Parser/XMLParser.h"
#include "Swiften/Parser/XMLParserClient.h"
#include "Swiften/Parser/PlatformXMLParserFactory.h"

namespace Swift {
	class PayloadsParserTester : public XMLParserClient {
		public:
			PayloadsParserTester() : level(0) {
				xmlParser = PlatformXMLParserFactory().createXMLParser(this);
			}

			~PayloadsParserTester() {
				delete xmlParser;
			}

			bool parse(const String& data) {
				return xmlParser->parse(data);
			}

			virtual void handleStartElement(const String& element, const String& ns, const AttributeMap& attributes) {
				if (level == 0) {
					CPPUNIT_ASSERT(!payloadParser.get());
					PayloadParserFactory* payloadParserFactory = factories.getPayloadParserFactory(element, ns, attributes);
					CPPUNIT_ASSERT(payloadParserFactory);
					payloadParser.reset(payloadParserFactory->createPayloadParser());
				}
				payloadParser->handleStartElement(element, ns, attributes);
				level++;
			}

			virtual void handleEndElement(const String& element, const String& ns) {
				level--;
				payloadParser->handleEndElement(element, ns);
			}

			virtual void handleCharacterData(const String& data) {
				payloadParser->handleCharacterData(data);
			}

			boost::shared_ptr<Payload> getPayload() const {
				return payloadParser->getPayload();
			}

		private:
			XMLParser* xmlParser;
			FullPayloadParserFactoryCollection factories;
			std::auto_ptr<PayloadParser> payloadParser;
			int level;
	};
}
