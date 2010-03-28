#ifndef SWIFTEN_XMPPPARSER_H
#define SWIFTEN_XMPPPARSER_H

#include <boost/shared_ptr.hpp>
#include <boost/noncopyable.hpp>

#include "Swiften/Parser/XMLParserClient.h"
#include "Swiften/Parser/AttributeMap.h"

namespace Swift {
	class XMLParser;
	class XMPPParserClient;
	class String;
	class ElementParser;
	class PayloadParserFactoryCollection;

	class XMPPParser : public XMLParserClient, boost::noncopyable {
		public:
			XMPPParser(
					XMPPParserClient* parserClient, 
					PayloadParserFactoryCollection* payloadParserFactories);
			~XMPPParser();

			bool parse(const String&);

		private:
			virtual void handleStartElement(
					const String& element, 
					const String& ns, 
					const AttributeMap& attributes);
			virtual void handleEndElement(const String& element, const String& ns);
			virtual void handleCharacterData(const String& data);

			bool inStream() const {
				return currentDepth_ > 0;
			}

			bool inElement() const {
				return currentDepth_ > 1;
			}

			ElementParser* createElementParser(const String& element, const String& xmlns);

		private:
			XMLParser* xmlParser_;
			XMPPParserClient* client_;
			PayloadParserFactoryCollection* payloadParserFactories_;
			int currentDepth_;
			ElementParser* currentElementParser_;
			bool parseErrorOccurred_;
	};
}

#endif
