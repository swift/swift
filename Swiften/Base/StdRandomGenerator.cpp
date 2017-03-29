/*
 * Copyright (c) 2012-2017 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#include <Swiften/Base/StdRandomGenerator.h>

#include <algorithm>
#include <functional>

namespace {
    template<class T = std::mt19937, std::size_t N = T::state_size>
    typename std::enable_if<!!N, T>::type createSeededRandomEngine() {
        typename T::result_type random_data[N];
        std::random_device source;
        std::generate(std::begin(random_data), std::end(random_data), std::ref(source));
        std::seed_seq seeds(std::begin(random_data), std::end(random_data));
        return T(seeds);
    }
}

namespace Swift {

StdRandomGenerator::StdRandomGenerator() : generator(createSeededRandomEngine()) {
}

int StdRandomGenerator::generateRandomInteger(int maximum) {
    std::uniform_int_distribution<> distribution(0, maximum);
    return distribution(generator);
}

}
