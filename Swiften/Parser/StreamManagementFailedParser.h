/*
 * Copyright (c) 2010 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <Swiften/Parser/GenericElementParser.h>
#include <Swiften/Elements/StreamManagementFailed.h>

namespace Swift {
	class StreamManagementFailedParser : public GenericElementParser<StreamManagementFailed> {
		public:
			StreamManagementFailedParser() : GenericElementParser<StreamManagementFailed>() {}
	};
}
