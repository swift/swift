#ifndef SWIFTEN_TLSProceedParser_H
#define SWIFTEN_TLSProceedParser_H

#include "Swiften/Parser/GenericElementParser.h"
#include "Swiften/Elements/TLSProceed.h"

namespace Swift {
	class TLSProceedParser : public GenericElementParser<TLSProceed> {
		public:
			TLSProceedParser() : GenericElementParser<TLSProceed>() {}
	};
}

#endif
