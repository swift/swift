/*
 * Copyright (c) 2010 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#pragma once

#include <cppunit/extensions/HelperMacros.h>

#include <Swiften/Parser/PayloadParsers/FullPayloadParserFactoryCollection.h>
#include <Swiften/Parser/XMLParser.h>
#include <Swiften/Parser/XMLParserClient.h>
#include <Swiften/Parser/PlatformXMLParserFactory.h>
#include <Swiften/Elements/Payload.h>
#include <Swiften/Parser/PayloadParser.h>

namespace Swift {
	class PayloadsParserTester : public XMLParserClient {
		public:
			PayloadsParserTester() : level(0) {
				xmlParser = PlatformXMLParserFactory().createXMLParser(this);
			}

			~PayloadsParserTester() {
				delete xmlParser;
			}

			bool parse(const std::string& data) {
				return xmlParser->parse(data);
			}

			virtual void handleStartElement(const std::string& element, const std::string& ns, const AttributeMap& attributes) {
				if (level == 0) {
					CPPUNIT_ASSERT(!payloadParser.get());
					PayloadParserFactory* payloadParserFactory = factories.getPayloadParserFactory(element, ns, attributes);
					CPPUNIT_ASSERT(payloadParserFactory);
					payloadParser.reset(payloadParserFactory->createPayloadParser());
				}
				payloadParser->handleStartElement(element, ns, attributes);
				level++;
			}

			virtual void handleEndElement(const std::string& element, const std::string& ns) {
				level--;
				payloadParser->handleEndElement(element, ns);
			}

			virtual void handleCharacterData(const std::string& data) {
				payloadParser->handleCharacterData(data);
			}

			boost::shared_ptr<Payload> getPayload() const {
				return payloadParser->getPayload();
			}

			template<typename T>
			boost::shared_ptr<T> getPayload() const {
				return boost::dynamic_pointer_cast<T>(payloadParser->getPayload());
			}

		private:
			XMLParser* xmlParser;
			FullPayloadParserFactoryCollection factories;
			boost::shared_ptr<PayloadParser> payloadParser;
			int level;
	};
}
