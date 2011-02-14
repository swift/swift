/*
 * Copyright (c) 2010 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#ifndef SWIFTEN_IDGenerator_H
#define SWIFTEN_IDGenerator_H

#include <string>

namespace Swift {
	class IDGenerator {
		public:
			IDGenerator();

			std::string generateID();

		private:
			std::string currentID_;
	};
}

#endif
