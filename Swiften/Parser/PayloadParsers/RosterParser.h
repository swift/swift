#ifndef SWIFTEN_RosterParser_H
#define SWIFTEN_RosterParser_H

#include "Swiften/Elements/RosterPayload.h"
#include "Swiften/Parser/GenericPayloadParser.h"

namespace Swift {
	class RosterParser : public GenericPayloadParser<RosterPayload> {
		public:
			RosterParser();

			virtual void handleStartElement(const String& element, const String&, const AttributeMap& attributes);
			virtual void handleEndElement(const String& element, const String&);
			virtual void handleCharacterData(const String& data);

		private:
			enum Level { 
				TopLevel = 0, 
				PayloadLevel = 1,
				ItemLevel = 2
			};
			int level_;
			bool inItem_;
			RosterItemPayload currentItem_;
			String currentText_;
	};
}

#endif
