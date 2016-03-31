/*
 * Copyright (c) 2010 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#include <Swiften/MUC/MUCRegistry.h>

#include <Swiften/Base/Algorithm.h>

namespace Swift {

MUCRegistry::~MUCRegistry() {
}

bool MUCRegistry::isMUC(const JID& j) const {
    return std::find(mucs.begin(), mucs.end(), j) != mucs.end();
}

void MUCRegistry::addMUC(const JID& j) {
    mucs.push_back(j);
}

void MUCRegistry::removeMUC(const JID& j) {
    erase(mucs, j);
}


}
