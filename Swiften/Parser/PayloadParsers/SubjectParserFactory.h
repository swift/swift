/*
 * Copyright (c) 2010 Kevin Smith
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#pragma once

#include "Swiften/Parser/GenericPayloadParserFactory.h"
#include "Swiften/Parser/PayloadParsers/SubjectParser.h"

namespace Swift {
	class SubjectParserFactory : public GenericPayloadParserFactory<SubjectParser> {
		public:
			SubjectParserFactory() : GenericPayloadParserFactory<SubjectParser>("subject") {}
	};
}

