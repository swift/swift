/*
 * Copyright (c) 2010 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#include <cassert>
#include <algorithm>

#include <Swiften/Base/String.h>

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

std::vector<unsigned int> String::getUnicodeCodePoints(const std::string& s) {
	std::vector<unsigned int> result;
	for (size_t i = 0; i < s.size();) {
		unsigned int codePoint = 0;
		char firstChar = s[i];
		size_t length = sequenceLength(firstChar);

		// First character is special
		size_t firstCharBitSize = 7 - length;
		if (length == 1) {
			firstCharBitSize = 7;
		}
		codePoint = firstChar & ((1<<(firstCharBitSize+1)) - 1);

		for (size_t j = 1; j < length; ++j) {
			codePoint = (codePoint<<6) | (s[i+j] & 0x3F);
		}
		result.push_back(codePoint);
		i += length;
	}
	return result;
}


std::pair<std::string,std::string> String::getSplittedAtFirst(const std::string& s, char c) {
	assert((c & 0x80) == 0);
	size_t firstMatch = s.find(c);
	if (firstMatch != s.npos) {
		return std::make_pair(s.substr(0,firstMatch),s.substr(firstMatch+1,s.npos));
	}
	else {
		return std::make_pair(s, "");
	}
}

void String::replaceAll(std::string& src, char c, const std::string& s) {
	size_t lastPos = 0;
	size_t matchingIndex = 0;
	while ((matchingIndex = src.find(c, lastPos)) != src.npos) {
		src.replace(matchingIndex, 1, s);
		lastPos = matchingIndex + s.size();
	}
}

std::vector<std::string> String::split(const std::string& s, char c) {
	assert((c & 0x80) == 0);
	std::vector<std::string> result;
	std::string accumulator;
	for (size_t i = 0; i < s.size(); ++i) {
		if (s[i] == c) {
			result.push_back(accumulator);
			accumulator = "";
		}
		else {
			accumulator += s[i];
		}
	}
	result.push_back(accumulator);
	return result;
}

}
