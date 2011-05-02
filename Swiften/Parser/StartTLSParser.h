/*
 * Copyright (c) 2010 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
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
