/*
 * Copyright (c) 2010 Kevin Smith
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#pragma once

#include <string>

namespace Swift {
	class XMLConsoleWidget {
		public:
			virtual ~XMLConsoleWidget();

			virtual void handleDataRead(const std::string& data) = 0;
			virtual void handleDataWritten(const std::string& data) = 0;

			virtual void show() = 0;
			virtual void activate() = 0;
	};
}
