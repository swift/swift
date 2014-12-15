/*
 * Copyright (c) 2010 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <Swiften/Parser/GenericElementParser.h>
#include <Swiften/Elements/EnableStreamManagement.h>

namespace Swift {
	class EnableStreamManagementParser : public GenericElementParser<EnableStreamManagement> {
		public:
			EnableStreamManagementParser() : GenericElementParser<EnableStreamManagement>() {}
	};
}
