/*
 * Copyright (c) 2010 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <string>

#include <Swiften/Base/API.h>

namespace Swift {
	class SWIFTEN_API SimpleIDGenerator {
		public:
			SimpleIDGenerator();

			std::string generateID();

		private:
			std::string currentID;
	};
}
