/*
 * Copyright (c) 2010 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#include <cassert>
#include <algorithm>

#include "Swiften/Base/String.h"

namespace Swift {

static inline size_t sequenceLength(char firstByte) {
	if ((firstByte & 0x80) == 0) {
		return 1;
	}
	if ((firstByte & 0xE0) == 0xC0) {
		return 2;
	}
	if ((firstByte & 0xF0) == 0xE0) {
		return 3;
	}
	if ((firstByte & 0xF8) == 0xF0) {
		return 4;
	}
	if ((firstByte & 0xFC) == 0xF8) {
		return 5;
	}
	if ((firstByte & 0xFE) == 0xFC) {
		return 6;
	}
	assert(false);
	return 1;
}

std::vector<unsigned int> String::getUnicodeCodePoints() const {
	std::vector<unsigned int> result;
	for (size_t i = 0; i < data_.size();) {
		unsigned int codePoint = 0;
		char firstChar = data_[i];
		size_t length = sequenceLength(firstChar);

		// First character is special
		size_t firstCharBitSize = 7 - length;
		if (length == 1) {
			firstCharBitSize = 7;
		}
		codePoint = firstChar & ((1<<(firstCharBitSize+1)) - 1);

		for (size_t j = 1; j < length; ++j) {
			codePoint = (codePoint<<6) | (data_[i+j] & 0x3F);
		}
		result.push_back(codePoint);
		i += length;
	}
	return result;
}


std::pair<String,String> String::getSplittedAtFirst(char c) const {
	assert((c & 0x80) == 0);
	size_t firstMatch = data_.find(c);
	if (firstMatch != data_.npos) {
		return std::make_pair(data_.substr(0,firstMatch),data_.substr(firstMatch+1,data_.npos));
	}
	else {
		return std::make_pair(*this, "");
	}
}

void String::replaceAll(char c, const String& s) {
	size_t lastPos = 0;
	size_t matchingIndex = 0;
	while ((matchingIndex = data_.find(c, lastPos)) != data_.npos) {
		data_.replace(matchingIndex, 1, s.data_);
		lastPos = matchingIndex + s.data_.size();
	}
}

std::vector<String> String::split(char c) const {
	assert((c & 0x80) == 0);
	std::vector<String> result;
	String accumulator;
	for (size_t i = 0; i < data_.size(); ++i) {
		if (data_[i] == c) {
			result.push_back(accumulator);
			accumulator = "";
		}
		else {
			accumulator += data_[i];
		}
	}
	result.push_back(accumulator);
	return result;
}

}
