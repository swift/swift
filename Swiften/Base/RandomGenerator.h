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

			virtual int generateWeighedRandomNumber(std::vector<double>::const_iterator probabilities_begin, std::vector<double>::const_iterator probabilities_end) = 0;
	};
}
