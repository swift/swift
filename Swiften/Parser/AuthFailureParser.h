/*
 * Copyright (c) 2010 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <Swiften/Parser/GenericElementParser.h>
#include <Swiften/Elements/AuthFailure.h>

namespace Swift {
	class AuthFailureParser : public GenericElementParser<AuthFailure> {
		public:
			AuthFailureParser() : GenericElementParser<AuthFailure>() {}
	};
}
