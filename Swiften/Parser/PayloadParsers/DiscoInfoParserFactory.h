#ifndef SWIFTEN_DiscoInfoParserFactory_H
#define SWIFTEN_DiscoInfoParserFactory_H

#include "Swiften/Parser/GenericPayloadParserFactory.h"
#include "Swiften/Parser/PayloadParsers/DiscoInfoParser.h"

namespace Swift {
	class DiscoInfoParserFactory : public GenericPayloadParserFactory<DiscoInfoParser> {
		public:
			DiscoInfoParserFactory() : GenericPayloadParserFactory<DiscoInfoParser>("query", "http://jabber.org/protocol/disco#info") {}
	};
}

#endif
