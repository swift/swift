#ifndef SWIFTEN_BodyParser_H
#define SWIFTEN_BodyParser_H

#include "Swiften/Elements/Body.h"
#include "Swiften/Parser/GenericPayloadParser.h"

namespace Swift {
	class BodyParser : public GenericPayloadParser<Body> {
		public:
			BodyParser();

			virtual void handleStartElement(const String& element, const String&, const AttributeMap& attributes);
			virtual void handleEndElement(const String& element, const String&);
			virtual void handleCharacterData(const String& data);

		private:
			int level_;
			String text_;
	};
}

#endif
