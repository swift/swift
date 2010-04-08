/*
 * Copyright (c) 2010 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#ifndef SWIFTEN_AuthFailureParser_H
#define SWIFTEN_AuthFailureParser_H

#include "Swiften/Parser/GenericElementParser.h"
#include "Swiften/Elements/AuthFailure.h"

namespace Swift {
	class AuthFailureParser : public GenericElementParser<AuthFailure> {
		public:
			AuthFailureParser() : GenericElementParser<AuthFailure>() {}
	};
}

#endif
