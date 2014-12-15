/*
 * Copyright (c) 2013 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <Swiften/Base/Override.h>
#include <Sluift/Terminal.h>

namespace Swift {
	class StandardTerminal : public Terminal {
		public:
			StandardTerminal();
			virtual ~StandardTerminal();

			virtual boost::optional<std::string> readLine(const std::string& prompt) SWIFTEN_OVERRIDE;
			virtual void printError(const std::string& message) SWIFTEN_OVERRIDE;
			virtual void addToHistory(const std::string& command) SWIFTEN_OVERRIDE;
	};
}
