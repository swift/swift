/*
 * Copyright (c) 2010 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
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
