/*
 * Copyright (c) 2010-2012 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#include <Swiften/IDN/IDNA.h>

#include <vector>
#include <cstdlib>
#if defined(HAVE_ICU)
#pragma GCC diagnostic ignored "-Wold-style-cast"
#include <Swiften/IDN/ICUConverter.h>
#include <unicode/uidna.h>
#elif defined(HAVE_LIBIDN)
#include <stringprep.h>
#include <idna.h>
#endif
#include <Swiften/Base/ByteArray.h>
#include <boost/shared_ptr.hpp>

namespace Swift {

std::string IDNA::getEncoded(const std::string& domain) {
#if defined(HAVE_ICU)
	UErrorCode status = U_ZERO_ERROR;
	ICUConverter converter;

	ICUConverter::ICUString icuInput = converter.convertToICUString(domain);
	ICUConverter::ICUString icuResult;
	icuResult.resize(icuInput.size());
	UParseError parseError;
	int icuResultLength = uidna_IDNToASCII(vecptr(icuInput), icuInput.size(), vecptr(icuResult), icuResult.size(), UIDNA_DEFAULT, &parseError, &status);
	if (status == U_BUFFER_OVERFLOW_ERROR) {
		status = U_ZERO_ERROR;
		icuResult.resize(icuResultLength);
		icuResultLength = uidna_IDNToASCII(vecptr(icuInput), icuInput.size(), vecptr(icuResult), icuResult.size(), UIDNA_DEFAULT, &parseError, &status);
	}
	if (U_FAILURE(status)) {
		return domain;
	}
	icuResult.resize(icuResultLength);

	return converter.convertToString(icuResult);

#elif defined(HAVE_LIBIDN)


	char* output;
	if (idna_to_ascii_8z(domain.c_str(), &output, 0) == IDNA_SUCCESS) {
		std::string result(output);
		free(output);
		return result;
	}
	else {
		return domain;
	}
#endif
}

}
