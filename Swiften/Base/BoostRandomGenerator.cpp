/*
 * Copyright (c) 2012 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#include <Swiften/Base/BoostRandomGenerator.h>

#include <numeric>
#include <boost/random/uniform_int.hpp>
#include <boost/random/variate_generator.hpp>
#include <ctime>

namespace Swift {

BoostRandomGenerator::BoostRandomGenerator() {
	// FIXME: Not a good seed
	generator.seed(static_cast<unsigned int>(std::time(0)));
}

int BoostRandomGenerator::generateRandomInteger(int maximum) {
	boost::uniform_int<> distribution(0, maximum);
	return distribution(generator);
}

}
