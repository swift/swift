/*
 * Copyright (c) 2012 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <Swiften/Base/API.h>
#include <vector>

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
