/*
 * Copyright (c) 2012-2016 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <boost/random/mersenne_twister.hpp>

#include <Swiften/Base/API.h>
#include <Swiften/Base/Override.h>
#include <Swiften/Base/RandomGenerator.h>

namespace Swift {
    class SWIFTEN_API BoostRandomGenerator : public RandomGenerator {
        public:
            BoostRandomGenerator();

            int generateRandomInteger(int max) SWIFTEN_OVERRIDE;

        private:
            boost::mt19937 generator;
    };
}
