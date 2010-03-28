#ifndef SWIFTEN_StartSessionParserFactory_H
#define SWIFTEN_StartSessionParserFactory_H

#include "Swiften/Parser/GenericPayloadParserFactory.h"
#include "Swiften/Parser/PayloadParsers/StartSessionParser.h"

namespace Swift {
	class StartSessionParserFactory : public GenericPayloadParserFactory<StartSessionParser> {
		public:
			StartSessionParserFactory() : GenericPayloadParserFactory<StartSessionParser>("session", "urn:ietf:params:xml:ns:xmpp-session") {}
	};
}

#endif
