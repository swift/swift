/*
 * Copyright (c) 2010-2013 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#include <Swiften/Base/Platform.h>

#include <cassert>
#include <algorithm>
#include <sstream>
#include <iomanip>
#ifdef SWIFTEN_PLATFORM_WINDOWS
#include <windows.h>
#endif

#include <Swiften/Base/String.h>
#include <Swiften/Base/ByteArray.h>

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

std::string String::convertIntToHexString(int h) {
	std::stringstream ss;
	ss << std::setbase(16);
	ss << h;
	return ss.str();
}

int String::convertHexStringToInt(const std::string& s) {
	std::stringstream ss;
	int h;
	ss << std::setbase(16);
	ss << s;
	ss >> h;
	return h;
}


#ifdef SWIFTEN_PLATFORM_WINDOWS
std::string convertWStringToString(const std::wstring& s) {
	int utf8Size = WideCharToMultiByte(CP_UTF8, 0, s.c_str(), -1, NULL, 0, NULL, NULL);
	if (utf8Size < 0) {
		throw std::runtime_error("Conversion error");
	}
	std::vector<char> utf8Data(utf8Size);
	int result = WideCharToMultiByte(
			CP_UTF8, 0, s.c_str(), -1, vecptr(utf8Data), utf8Data.size(), NULL, NULL);
	if (result < 0) {
		throw std::runtime_error("Conversion error");
	}
	return std::string(vecptr(utf8Data), utf8Size-1 /* trailing 0 character */);
}

std::wstring convertStringToWString(const std::string& s) {
	int utf16Size = MultiByteToWideChar(CP_UTF8, 0, s.c_str(), -1, NULL, 0);
	if (utf16Size < 0) {
		throw std::runtime_error("Conversion error");
	}
	std::vector<wchar_t> utf16Data(utf16Size);
	int result = MultiByteToWideChar(
			CP_UTF8, 0, s.c_str(), -1, vecptr(utf16Data), utf16Data.size());
	if (result < 0) {
		throw std::runtime_error("Conversion error");
	}
	return std::wstring(vecptr(utf16Data), utf16Size-1 /* trailing 0 character */);
}
#endif

}
