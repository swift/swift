/*
 * Copyright (c) 2010 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
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
