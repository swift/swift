#ifndef SWIFTEN_StartTLSParser_H
#define SWIFTEN_StartTLSParser_H

#include "Swiften/Parser/GenericElementParser.h"
#include "Swiften/Elements/StartTLSRequest.h"

namespace Swift {
	class StartTLSParser : public GenericElementParser<StartTLSRequest> {
		public:
			StartTLSParser() : GenericElementParser<StartTLSRequest>() {}
	};
}

#endif
