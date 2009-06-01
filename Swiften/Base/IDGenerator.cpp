#include "Swiften/Base/IDGenerator.h"

namespace Swift {

IDGenerator::IDGenerator() {
}

String IDGenerator::generateID() {
	bool carry = true;
	size_t i = 0;
	while (carry && i < currentID_.getUTF8Size()) {
		char c = currentID_.getUTF8String()[i];
		if (c >= 'z') {
			currentID_.getUTF8String()[i] = 'a';
		}
		else {
			currentID_.getUTF8String()[i] = c+1;
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
