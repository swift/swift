/*
 * Copyright (c) 2010 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <Swiften/Parser/GenericElementParser.h>
#include <Swiften/Elements/UnknownElement.h>

namespace Swift {
	class UnknownElementParser : public GenericElementParser<UnknownElement> {
		public:
			UnknownElementParser() : GenericElementParser<UnknownElement>() {}
	};
}
