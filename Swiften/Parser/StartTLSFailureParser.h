/*
 * Copyright (c) 2010 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <Swiften/Parser/GenericElementParser.h>
#include <Swiften/Elements/StartTLSFailure.h>

namespace Swift {
	class StartTLSFailureParser : public GenericElementParser<StartTLSFailure> {
		public:
			StartTLSFailureParser() : GenericElementParser<StartTLSFailure>() {}
	};
}
