/*
 * Copyright (c) 2010 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

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
