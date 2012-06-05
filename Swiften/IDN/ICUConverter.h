/*
 * Copyright (c) 2012 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#pragma once

#pragma GCC diagnostic ignored "-Wold-style-cast"

#include <unicode/ustring.h>
#include <cassert>
#include <string>
#include <vector>
#include <Swiften/Base/ByteArray.h>
#include <Swiften/Base/SafeByteArray.h>

namespace Swift {

	class ICUConverter {
		public:
			typedef std::vector<UChar, SafeAllocator<UChar> > ICUString;

			template<typename T>
			ICUString convertToICUString(const T& s) {
				ICUString result;
				result.resize(s.size());
				UErrorCode status = U_ZERO_ERROR;
				int icuResultLength = result.size();
				u_strFromUTF8Lenient(vecptr(result), result.size(), &icuResultLength, toConstCharArray(s), s.size(), &status);
				if (status == U_BUFFER_OVERFLOW_ERROR) {
					status = U_ZERO_ERROR;
					result.resize(icuResultLength);
					u_strFromUTF8Lenient(vecptr(result), result.size(), &icuResultLength, toConstCharArray(s), s.size(), &status);
				}
				if (U_FAILURE(status)) {
					return ICUString();
				}
				result.resize(icuResultLength);
				return result;
			}

			std::string convertToString(const ICUString& input) {
				return std::string(vecptr(convertToArray(input)));
			}

			std::vector<char, SafeAllocator<char> > convertToArray(const ICUString& input) {
				std::vector<char, SafeAllocator<char> > result;
				result.resize(input.size());
				UErrorCode status = U_ZERO_ERROR;
				int inputLength = result.size();
				u_strToUTF8(vecptr(result), result.size(), &inputLength, vecptr(input), input.size(), &status);
				if (status == U_BUFFER_OVERFLOW_ERROR) {
					status = U_ZERO_ERROR;
					result.resize(inputLength);
					u_strToUTF8(vecptr(result), result.size(), &inputLength, vecptr(input), input.size(), &status);
				}
				if (U_FAILURE(status)) {
					return std::vector<char, SafeAllocator<char> >();
				}

				result.resize(inputLength + 1);
				result[result.size() - 1] = '\0';
				return result;
			}

		private:
			static const char* toConstCharArray(const std::string& input) {
				return input.c_str();
			}

			static const char* toConstCharArray(const std::vector<unsigned char, SafeAllocator<unsigned char> >& input) {
				return reinterpret_cast<const char*>(vecptr(input));
			}
	};

}
