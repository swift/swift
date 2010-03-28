#ifndef SWIFTEN_MESSAGEPARSER_H
#define SWIFTEN_MESSAGEPARSER_H

#include "Swiften/Parser/GenericStanzaParser.h"
#include "Swiften/Elements/Message.h"

namespace Swift {
	class MessageParser : public GenericStanzaParser<Message> {
		public:
			MessageParser(PayloadParserFactoryCollection* factories);

		private:
			virtual void handleStanzaAttributes(const AttributeMap&);
	};
}

#endif
