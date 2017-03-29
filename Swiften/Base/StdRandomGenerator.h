/*
 * Copyright (c) 2012-2017 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <random>

#include <Swiften/Base/API.h>
#include <Swiften/Base/Override.h>
#include <Swiften/Base/RandomGenerator.h>

namespace Swift {
    class SWIFTEN_API StdRandomGenerator : public RandomGenerator {
        public:
            StdRandomGenerator();

            int generateRandomInteger(int max) SWIFTEN_OVERRIDE;

        private:
            std::mt19937 generator;
    };
}
