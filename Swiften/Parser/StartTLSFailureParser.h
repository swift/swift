#ifndef SWIFTEN_StartTLSFailureParser_H
#define SWIFTEN_StartTLSFailureParser_H

#include "Swiften/Parser/GenericElementParser.h"
#include "Swiften/Elements/StartTLSFailure.h"

namespace Swift {
	class StartTLSFailureParser : public GenericElementParser<StartTLSFailure> {
		public:
			StartTLSFailureParser() : GenericElementParser<StartTLSFailure>() {}
	};
}

#endif
