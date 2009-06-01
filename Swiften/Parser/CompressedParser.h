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
