#ifndef SWIFTEN_XMLPayloadParser_H
#define SWIFTEN_XMLPayloadParser_H

#include "Swiften/Parser/PayloadParser.h"
#include "Swiften/Parser/XMLParserClient.h"
#include "Swiften/Parser/XMLParser.h"
#include "Swiften/Parser/PlatformXMLParserFactory.h"

namespace Swift {
	class XMLPayloadParser : public XMLParserClient {
		public:
			XMLPayloadParser(PayloadParser* payloadParser) : 
					payloadParser_(payloadParser) {
				xmlParser_ = PlatformXMLParserFactory().createXMLParser(this);
			}

			~XMLPayloadParser() {
				delete xmlParser_;
			}

			bool parse(const String& data) {
				return xmlParser_->parse(data);
			}

			virtual void handleStartElement(const String& element, const String& ns, const AttributeMap& attributes) {
				payloadParser_->handleStartElement(element, ns, attributes);
			}

			virtual void handleEndElement(const String& element, const String& ns) {
				payloadParser_->handleEndElement(element, ns);
			}

			virtual void handleCharacterData(const String& data) {
				payloadParser_->handleCharacterData(data);
			}

		private:
			XMLParser* xmlParser_;
			PayloadParser* payloadParser_;
	};
}
#endif
