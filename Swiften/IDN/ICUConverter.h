/*
 * Copyright (c) 2012 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#pragma once

#pragma GCC diagnostic ignored "-Wold-style-cast"

#include <unicode/ucnv.h>
#include <cassert>
#include <string>
#include <vector>
#include <boost/shared_ptr.hpp>
#include <Swiften/Base/ByteArray.h>
#include <Swiften/Base/SafeByteArray.h>

namespace Swift {

	class ICUConverter {
		public:
			typedef std::vector<UChar, SafeAllocator<UChar> > ICUString;

			ICUConverter() {
				UErrorCode status = U_ZERO_ERROR;
				icuConverter.reset(ucnv_open("utf-8", &status), ucnv_close);
				assert(U_SUCCESS(status));
			}

			template<typename T>
			ICUString convertToICUString(const T& s) {
				ICUString result;
				result.resize(s.size());
				UErrorCode status = U_ZERO_ERROR;
				int icuResultLength = ucnv_toUChars(icuConverter.get(), vecptr(result), result.size(), toConstCharArray(s), s.size(), &status);
				if (status == U_BUFFER_OVERFLOW_ERROR) {
					status = U_ZERO_ERROR;
					result.resize(icuResultLength);
					icuResultLength = ucnv_toUChars(icuConverter.get(), vecptr(result), result.size(), toConstCharArray(s), s.size(), &status);
				}
				assert(U_SUCCESS(status));
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
				int inputLength = ucnv_fromUChars(icuConverter.get(), vecptr(result), result.size(), vecptr(input), input.size(), &status);
				if (status == U_BUFFER_OVERFLOW_ERROR) {
					status = U_ZERO_ERROR;
					result.resize(inputLength);
					inputLength = ucnv_fromUChars(icuConverter.get(), vecptr(result), result.size(), vecptr(input), input.size(), &status);
				}
				assert(U_SUCCESS(status));

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

		private:
			boost::shared_ptr<UConverter> icuConverter;
	};

}
