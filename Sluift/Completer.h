/*
 * Copyright (c) 2013 Remko Tronçon
 * Licensed under the GNU General Public License.
 * See the COPYING file for more information.
 */

#pragma once

#include <string>
#include <vector>

namespace Swift {
	class Completer {
		public:
			virtual ~Completer();

			virtual std::vector<std::string> getCompletions(const std::string& buffer, int start, int end) = 0;
	};
}
