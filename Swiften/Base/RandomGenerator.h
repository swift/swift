/*
 * Copyright (c) 2012-2016 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <vector>

#include <Swiften/Base/API.h>

namespace Swift {
    class SWIFTEN_API RandomGenerator {
        public:
            virtual ~RandomGenerator();

            /**
             * Generates a random integer between 0 and 'max',
             * 'max' inclusive.
             */
            virtual int generateRandomInteger(int max) = 0;
    };
}
