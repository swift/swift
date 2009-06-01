#ifndef SWIFTEN_StatusParser_H
#define SWIFTEN_StatusParser_H

#include "Swiften/Elements/Status.h"
#include "Swiften/Parser/GenericPayloadParser.h"

namespace Swift {
	class StatusParser : public GenericPayloadParser<Status> {
		public:
			StatusParser();

			virtual void handleStartElement(const String& element, const String&, const AttributeMap& attributes);
			virtual void handleEndElement(const String& element, const String&);
			virtual void handleCharacterData(const String& data);

		private:
			int level_;
			String text_;
	};
}

#endif
