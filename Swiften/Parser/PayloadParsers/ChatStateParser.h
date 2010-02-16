#pragma once

#include "Swiften/Elements/ChatState.h"
#include "Swiften/Parser/GenericPayloadParser.h"

namespace Swift {
	class ChatStateParser : public GenericPayloadParser<ChatState> {
		public:
			ChatStateParser();

			virtual void handleStartElement(const String& element, const String&, const AttributeMap& attributes);
			virtual void handleEndElement(const String& element, const String&);
			virtual void handleCharacterData(const String& data);

		private:
			int level_;
	};
}
