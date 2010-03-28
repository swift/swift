#ifndef SWIFTEN_StanzaParser_H
#define SWIFTEN_StanzaParser_H

#include <boost/noncopyable.hpp>

#include "Swiften/Base/String.h"
#include "Swiften/Elements/Stanza.h"
#include "Swiften/Parser/ElementParser.h"
#include "Swiften/Parser/AttributeMap.h"

namespace Swift {
	class PayloadParser;
	class PayloadParserFactoryCollection;

	class StanzaParser : public ElementParser, public boost::noncopyable {
		public:
			StanzaParser(PayloadParserFactoryCollection* factories);
			~StanzaParser();

			void handleStartElement(const String& element, const String& ns, const AttributeMap& attributes);
			void handleEndElement(const String& element, const String& ns);
			void handleCharacterData(const String& data);

			virtual boost::shared_ptr<Element> getElement() const = 0;
			virtual void handleStanzaAttributes(const AttributeMap&) {}

			virtual boost::shared_ptr<Stanza> getStanza() const {
				return boost::dynamic_pointer_cast<Stanza>(getElement());
			}

		private:
			bool inPayload() const {
				return currentDepth_ > 1;
			}

			bool inStanza() const {
				return currentDepth_ > 0;
			}


		private:
			int currentDepth_;
			PayloadParserFactoryCollection* factories_;
			std::auto_ptr<PayloadParser> currentPayloadParser_;
	};
}

#endif
