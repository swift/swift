#ifndef SWIFTEN_StartSessionParser_H
#define SWIFTEN_StartSessionParser_H

#include "Swiften/Elements/StartSession.h"
#include "Swiften/Parser/GenericPayloadParser.h"

namespace Swift {
	class StartSessionParser : public GenericPayloadParser<StartSession> {
		public:
			StartSessionParser() {}

			virtual void handleStartElement(const String&, const String&, const AttributeMap&) {}
			virtual void handleEndElement(const String&, const String&) {}
			virtual void handleCharacterData(const String&) {}
	};
}

#endif
