/*
 * Copyright (c) 2010 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#ifndef SWIFTEN_COMPRESSEDPARSER_H
#define SWIFTEN_COMPRESSEDPARSER_H

#include "Swiften/Parser/GenericElementParser.h"
#include "Swiften/Elements/Compressed.h"

namespace Swift {
	class CompressedParser : public GenericElementParser<Compressed> {
		public:
			CompressedParser() : GenericElementParser<Compressed>() {}
	};
}

#endif
