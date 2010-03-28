#pragma once

#include "Swiften/Elements/VCardUpdate.h"
#include "Swiften/Parser/GenericPayloadParser.h"

namespace Swift {
	class SerializingParser;

	class VCardUpdateParser : public GenericPayloadParser<VCardUpdate> {
		public:
			VCardUpdateParser();

			virtual void handleStartElement(const String& element, const String&, const AttributeMap& attributes);
			virtual void handleEndElement(const String& element, const String&);
			virtual void handleCharacterData(const String& data);

		private:
			enum Level { 
				TopLevel = 0, 
				PayloadLevel = 1
			};
			int level_;
			String currentText_;
	};
}
