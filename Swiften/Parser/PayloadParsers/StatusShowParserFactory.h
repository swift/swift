/*
 * Copyright (c) 2010 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#ifndef SWIFTEN_StatusShowParserFactory_H
#define SWIFTEN_StatusShowParserFactory_H

#include "Swiften/Parser/GenericPayloadParserFactory.h"
#include "Swiften/Parser/PayloadParsers/StatusShowParser.h"

namespace Swift {
	class StatusShowParserFactory : public GenericPayloadParserFactory<StatusShowParser> {
		public:
			StatusShowParserFactory() : GenericPayloadParserFactory<StatusShowParser>("show") {}
	};
}

#endif
