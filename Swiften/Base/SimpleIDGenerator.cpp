/*
 * Copyright (c) 2010-2016 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#include <Swiften/Base/SimpleIDGenerator.h>

namespace Swift {

SimpleIDGenerator::SimpleIDGenerator() {
}

SimpleIDGenerator::~SimpleIDGenerator() {
}

std::string SimpleIDGenerator::generateID() {
    bool carry = true;
    size_t i = 0;
    while (carry && i < currentID.size()) {
        char c = currentID[i];
        if (c >= 'z') {
            currentID[i] = 'a';
        }
        else {
            currentID[i] = c+1;
            carry = false;
        }
        ++i;
    }
    if (carry) {
        currentID += 'a';
    }
    return currentID;
}

}
