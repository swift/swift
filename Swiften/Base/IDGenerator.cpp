/*
 * Copyright (c) 2010 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#include "Swiften/Base/IDGenerator.h"

namespace Swift {

IDGenerator::IDGenerator() {
}

std::string IDGenerator::generateID() {
	bool carry = true;
	size_t i = 0;
	while (carry && i < currentID_.size()) {
		char c = currentID_[i];
		if (c >= 'z') {
			currentID_[i] = 'a';
		}
		else {
			currentID_[i] = c+1;
			carry = false;
		}
		++i;
	}
	if (carry) {
		currentID_ += 'a';
	}
	return currentID_;
}

}
