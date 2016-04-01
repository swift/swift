/*
 * Copyright (c) 2012-2016 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#include <Swiften/Base/BoostRandomGenerator.h>

#include <ctime>
#include <numeric>

#include <boost/random/uniform_int.hpp>
#include <boost/random/variate_generator.hpp>

namespace Swift {

BoostRandomGenerator::BoostRandomGenerator() {
    // FIXME: Not a good seed
    generator.seed(static_cast<unsigned int>(std::time(nullptr)));
}

int BoostRandomGenerator::generateRandomInteger(int maximum) {
    boost::uniform_int<> distribution(0, maximum);
    return distribution(generator);
}

}
