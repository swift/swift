/*
 * Copyright (c) 2012 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#pragma once

#include <vector>

namespace Swift {
	class RandomGenerator {
		public:
			virtual ~RandomGenerator();

			virtual int generateRandomInteger(int max) = 0;
	};
}
