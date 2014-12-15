/*
 * Copyright (c) 2010 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <Swiften/Parser/GenericElementParser.h>
#include <Swiften/Elements/Compressed.h>

namespace Swift {
	class CompressedParser : public GenericElementParser<Compressed> {
		public:
			CompressedParser() : GenericElementParser<Compressed>() {}
	};
}
