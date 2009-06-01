#ifndef SWIFTEN_RosterParserFactory_H
#define SWIFTEN_RosterParserFactory_H

#include "Swiften/Parser/GenericPayloadParserFactory.h"
#include "Swiften/Parser/PayloadParsers/RosterParser.h"

namespace Swift {
	class RosterParserFactory : public GenericPayloadParserFactory<RosterParser> {
		public:
			RosterParserFactory() : GenericPayloadParserFactory<RosterParser>("query", "jabber:iq:roster") {}
	};
}

#endif
