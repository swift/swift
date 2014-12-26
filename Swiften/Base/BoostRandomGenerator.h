/*
 * Copyright (c) 2012-2015 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <Swiften/Base/RandomGenerator.h>
#include <Swiften/Base/Override.h>

#include <boost/random/mersenne_twister.hpp>

namespace Swift {
	class BoostRandomGenerator : public RandomGenerator {
		public:
			BoostRandomGenerator();

			int generateRandomInteger(int max) SWIFTEN_OVERRIDE;

		private:
			boost::mt19937 generator;
	};
}
