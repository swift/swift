#ifndef SWIFTEN_AUTHSUCCESSPARSER_H
#define SWIFTEN_AUTHSUCCESSPARSER_H

#include "Swiften/Parser/GenericElementParser.h"
#include "Swiften/Elements/AuthSuccess.h"

namespace Swift {
	class AuthSuccessParser : public GenericElementParser<AuthSuccess> {
		public:
			AuthSuccessParser() : GenericElementParser<AuthSuccess>() {}
	};
}

#endif
