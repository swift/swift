/*
 * Copyright (c) 2010 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#pragma once

#include <string>
#include <vector>
#include <sstream>

#define SWIFTEN_STRING_TO_CFSTRING(a) \
	CFStringCreateWithBytes(NULL, reinterpret_cast<const UInt8*>(a.c_str()), a.size(), kCFStringEncodingUTF8, false)

namespace Swift {
	namespace String {
			std::vector<unsigned int> getUnicodeCodePoints(const std::string&);
			std::pair<std::string, std::string> getSplittedAtFirst(const std::string&, char c);
			std::vector<std::string> split(const std::string&, char c);
			void replaceAll(std::string&, char c, const std::string& s);

			inline bool beginsWith(const std::string& s, char c) { 
				return s.size() > 0 && s[0] == c; 
			}

			inline bool endsWith(const std::string& s, char c) { 
				return s.size() > 0 && s[s.size()-1] == c; 
			}
	};

	class makeString {
		public:
			 template <typename T> makeString& operator<<(T const& v) {
					stream << v;
					return *this;
			 }

			 operator std::string() const {
					return stream.str();
			 }

		private:
			 std::ostringstream stream;
	};
}
