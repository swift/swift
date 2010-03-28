#ifndef SWIFTEN_UnknownElementParser_H
#define SWIFTEN_UnknownElementParser_H

#include "Swiften/Parser/GenericElementParser.h"
#include "Swiften/Elements/UnknownElement.h"

namespace Swift {
	class UnknownElementParser : public GenericElementParser<UnknownElement> {
		public:
			UnknownElementParser() : GenericElementParser<UnknownElement>() {}
	};
}

#endif
