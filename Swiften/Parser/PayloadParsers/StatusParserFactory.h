#ifndef SWIFTEN_StatusParserFactory_H
#define SWIFTEN_StatusParserFactory_H

#include "Swiften/Parser/GenericPayloadParserFactory.h"
#include "Swiften/Parser/PayloadParsers/StatusParser.h"

namespace Swift {
	class StatusParserFactory : public GenericPayloadParserFactory<StatusParser> {
		public:
			StatusParserFactory() : GenericPayloadParserFactory<StatusParser>("status") {}
	};
}

#endif
