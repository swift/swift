/*
 * Copyright (c) 2010 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#ifndef SWIFTEN_PriorityParserFactory_H
#define SWIFTEN_PriorityParserFactory_H

#include "Swiften/Parser/GenericPayloadParserFactory.h"
#include "Swiften/Parser/PayloadParsers/PriorityParser.h"

namespace Swift {
	class PriorityParserFactory : public GenericPayloadParserFactory<PriorityParser> {
		public:
			PriorityParserFactory() : GenericPayloadParserFactory<PriorityParser>("priority") {}
	};
}

#endif
