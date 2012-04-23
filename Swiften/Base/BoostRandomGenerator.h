/*
 * Copyright (c) 2012 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#pragma once

#include <Swiften/Base/RandomGenerator.h>

#include <boost/random/mersenne_twister.hpp>

namespace Swift {
	class BoostRandomGenerator : public RandomGenerator{
		public:
			int generateWeighedRandomNumber(std::vector<double>::const_iterator probabilities_begin, std::vector<double>::const_iterator probabilities_end);

		private:
			boost::mt19937 generator;
	};
}
