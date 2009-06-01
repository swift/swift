#ifndef SWIFTEN_PresenceParser_H
#define SWIFTEN_PresenceParser_H

#include "Swiften/Parser/GenericStanzaParser.h"
#include "Swiften/Elements/Presence.h"

namespace Swift {
	class PresenceParser : public GenericStanzaParser<Presence> {
		public:
			PresenceParser(PayloadParserFactoryCollection* factories);

		private:
			virtual void handleStanzaAttributes(const AttributeMap&);
	};
}

#endif
