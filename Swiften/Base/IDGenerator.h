/*
 * Copyright (c) 2010 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#ifndef SWIFTEN_IDGenerator_H
#define SWIFTEN_IDGenerator_H

#include "Swiften/Base/String.h"

namespace Swift {
	class IDGenerator {
		public:
			IDGenerator();

			String generateID();

		private:
			String currentID_;
	};
}

#endif
