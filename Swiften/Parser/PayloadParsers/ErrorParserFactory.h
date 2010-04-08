/*
 * Copyright (c) 2010 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#ifndef SWIFTEN_ErrorParserFactory_H
#define SWIFTEN_ErrorParserFactory_H

#include "Swiften/Parser/GenericPayloadParserFactory.h"
#include "Swiften/Parser/PayloadParsers/ErrorParser.h"

namespace Swift {
	class ErrorParserFactory : public GenericPayloadParserFactory<ErrorParser> {
		public:
			ErrorParserFactory() : GenericPayloadParserFactory<ErrorParser>("error") {}
	};
}

#endif
