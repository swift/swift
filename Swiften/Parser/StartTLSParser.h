/*
 * Copyright (c) 2010 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <Swiften/Parser/GenericElementParser.h>
#include <Swiften/Elements/StartTLSRequest.h>

namespace Swift {
	class StartTLSParser : public GenericElementParser<StartTLSRequest> {
		public:
			StartTLSParser() : GenericElementParser<StartTLSRequest>() {}
	};
}
