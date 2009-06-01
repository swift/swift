#ifndef SWIFTEN_SecurityLabelsCatalogParserFactory_H
#define SWIFTEN_SecurityLabelsCatalogParserFactory_H

#include "Swiften/Parser/GenericPayloadParserFactory.h"
#include "Swiften/Parser/PayloadParsers/SecurityLabelsCatalogParser.h"

namespace Swift {
	class SecurityLabelsCatalogParserFactory : public GenericPayloadParserFactory<SecurityLabelsCatalogParser> {
		public:
			SecurityLabelsCatalogParserFactory() : GenericPayloadParserFactory<SecurityLabelsCatalogParser>("catalog", "urn:xmpp:sec-label:catalog:0") {}
	};
}

#endif
