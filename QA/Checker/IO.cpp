/*
 * Copyright (c) 2010 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#include <QA/Checker/IO.h>

#include <algorithm>
#include <iostream>

std::ostream& operator<<(std::ostream& os, const Swift::ByteArray& s) {
	std::ios::fmtflags oldFlags = os.flags(); 
	os << std::hex;
	for (Swift::ByteArray::const_iterator i = s.begin(); i != s.end(); ++i) {
		if (*i >= 32 && *i < 127) {
			os << *i;
		}
		else {
			os << "\\x" << static_cast<unsigned int>(static_cast<unsigned char>(*i));
		}
	}
	os << std::endl;
	os.flags(oldFlags);
	return os;
}

std::ostream& operator<<(std::ostream& os, const Swift::SafeByteArray& s) {
	std::ios::fmtflags oldFlags = os.flags();
	os << std::hex;
	for (Swift::SafeByteArray::const_iterator i = s.begin(); i != s.end(); ++i) {
		if (*i >= 32 && *i < 127) {
			os << *i;
		}
		else {
			os << "\\x" << static_cast<unsigned int>(static_cast<unsigned char>(*i));
		}
	}
	os << std::endl;
	os.flags(oldFlags);
	return os;
}

std::ostream& operator<<(std::ostream& os, const std::vector<int>& s) {
	for (std::vector<int>::const_iterator i = s.begin(); i != s.end(); ++i) {
		os << *i << " ";
	}
	os << std::endl;
	return os;
}

std::ostream& operator<<(std::ostream& os, const std::vector<size_t>& s) {
	for (std::vector<size_t>::const_iterator i = s.begin(); i != s.end(); ++i) {
		os << *i << " ";
	}
	os << std::endl;
	return os;
}

bool operator==(const Swift::ByteArray& a, const Swift::ByteArray& b) {
	if (a.size() != b.size()) {
		return false;
	}
	return std::equal(a.begin(), a.end(), b.begin());
}
