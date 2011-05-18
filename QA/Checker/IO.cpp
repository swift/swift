/*
 * Copyright (c) 2010 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#include <QA/Checker/IO.h>

#include <iostream>

std::ostream& operator<<(std::ostream& os, const Swift::ByteArray& s) {
	std::ios::fmtflags oldFlags = os.flags(); 
	os << std::hex;
	for (Swift::ByteArray::const_iterator i = s.begin(); i != s.end(); ++i) {
		os << "0x" << static_cast<unsigned int>(static_cast<unsigned char>(*i));
		if (i + 1 < s.end()) {
			os << " ";
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
		os << "0x" << static_cast<unsigned int>(static_cast<unsigned char>(*i));
		if (i + 1 < s.end()) {
			os << " ";
		}
	}
	os << std::endl;
	os.flags(oldFlags);
	return os;
}

std::ostream& operator<<(std::ostream& os, const Swift::SafeString& s) {
	os << s.toString();
	return os;
}
