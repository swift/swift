/*
 * Copyright (c) 2012 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#pragma once

#include <Swiften/Base/API.h>
#include <vector>

namespace Swift {
	class SWIFTEN_API RandomGenerator {
		public:
			virtual ~RandomGenerator();

			virtual int generateRandomInteger(int max) = 0;
	};
}
