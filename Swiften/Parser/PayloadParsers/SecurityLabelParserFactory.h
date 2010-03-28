#ifndef SWIFTEN_SecurityLabelParserFactory_H
#define SWIFTEN_SecurityLabelParserFactory_H

#include "Swiften/Parser/GenericPayloadParserFactory.h"
#include "Swiften/Parser/PayloadParsers/SecurityLabelParser.h"

namespace Swift {
	class SecurityLabelParserFactory : public GenericPayloadParserFactory<SecurityLabelParser> {
		public:
			SecurityLabelParserFactory() : GenericPayloadParserFactory<SecurityLabelParser>("securitylabel", "urn:xmpp:sec-label:0") {}
	};
}

#endif
