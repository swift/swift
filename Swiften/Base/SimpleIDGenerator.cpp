/*
 * Copyright (c) 2010 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#include "Swiften/Base/SimpleIDGenerator.h"

namespace Swift {

SimpleIDGenerator::SimpleIDGenerator() {
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
