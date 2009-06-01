#ifndef SWIFTEN_SoftwareVersionParserFactory_H
#define SWIFTEN_SoftwareVersionParserFactory_H

#include "Swiften/Parser/GenericPayloadParserFactory.h"
#include "Swiften/Parser/PayloadParsers/SoftwareVersionParser.h"

namespace Swift {
	class SoftwareVersionParserFactory : public GenericPayloadParserFactory<SoftwareVersionParser> {
		public:
			SoftwareVersionParserFactory() : GenericPayloadParserFactory<SoftwareVersionParser>("query", "jabber:iq:version") {}
	};
}

#endif
