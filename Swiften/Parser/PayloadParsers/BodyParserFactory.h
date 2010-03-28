#ifndef SWIFTEN_BodyParserFACTORY_H
#define SWIFTEN_BodyParserFACTORY_H

#include "Swiften/Parser/GenericPayloadParserFactory.h"
#include "Swiften/Parser/PayloadParsers/BodyParser.h"

namespace Swift {
	class BodyParserFactory : public GenericPayloadParserFactory<BodyParser> {
		public:
			BodyParserFactory() : GenericPayloadParserFactory<BodyParser>("body") {}
	};
}

#endif
