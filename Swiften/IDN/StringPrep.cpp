/*
 * Copyright (c) 2010-2012 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#include <Swiften/IDN/StringPrep.h>

#if defined(HAVE_ICU)
#pragma GCC diagnostic ignored "-Wold-style-cast"
#include <Swiften/IDN/ICUConverter.h>
#include <unicode/usprep.h>
#include <unicode/ucnv.h>
#elif defined(HAVE_LIBIDN)
extern "C" {
	#include <stringprep.h>
};
#endif

#include <vector>
#include <cassert>
#include <Swiften/Base/SafeAllocator.h>
#include <boost/shared_ptr.hpp>

using namespace Swift;

#if defined(HAVE_ICU)

namespace {
	static UStringPrepProfileType getICUProfileType(StringPrep::Profile profile) {
		switch(profile) {
			case StringPrep::NamePrep: return USPREP_RFC3491_NAMEPREP; break;
			case StringPrep::XMPPNodePrep: return USPREP_RFC3920_NODEPREP; break;
			case StringPrep::XMPPResourcePrep: return USPREP_RFC3920_RESOURCEPREP; break;
			case StringPrep::SASLPrep: return USPREP_RFC4013_SASLPREP; break;
		}
		assert(false);
		return USPREP_RFC3491_NAMEPREP;
	}

	template<typename StringType>
	std::vector<char, SafeAllocator<char> > getStringPrepared(const StringType& s, StringPrep::Profile profile) {
		UErrorCode status = U_ZERO_ERROR;
		ICUConverter converter;

		boost::shared_ptr<UStringPrepProfile> icuProfile(usprep_openByType(getICUProfileType(profile), &status), usprep_close);
		assert(U_SUCCESS(status));

		ICUConverter::ICUString icuInput = converter.convertToICUString(s);
		ICUConverter::ICUString icuResult;
		UParseError parseError;
		icuResult.resize(icuInput.size());
		int icuResultLength = usprep_prepare(icuProfile.get(), vecptr(icuInput), icuInput.size(), vecptr(icuResult), icuResult.size(), USPREP_ALLOW_UNASSIGNED, &parseError, &status); icuResult.resize(icuResultLength);
		if (status == U_BUFFER_OVERFLOW_ERROR) {
			status = U_ZERO_ERROR;
			icuResult.resize(icuResultLength);
			icuResultLength = usprep_prepare(icuProfile.get(), vecptr(icuInput), icuInput.size(), vecptr(icuResult), icuResult.size(), USPREP_ALLOW_UNASSIGNED, &parseError, &status); icuResult.resize(icuResultLength);
		}
		if (U_FAILURE(status)) {
			return std::vector<char, SafeAllocator<char> >();
		}
		icuResult.resize(icuResultLength);

		return converter.convertToArray(icuResult);
	}
}

namespace Swift {

std::string StringPrep::getPrepared(const std::string& s, Profile profile) {
	if (s.empty()) {
		return "";
	}
	std::vector<char, SafeAllocator<char> > preparedData = getStringPrepared(s, profile);
	if (preparedData.empty()) {
		throw std::exception();
	}
	return std::string(vecptr(preparedData));
}

SafeByteArray StringPrep::getPrepared(const SafeByteArray& s, Profile profile) {
	if (s.empty()) {
		return SafeByteArray();
	}
	std::vector<char, SafeAllocator<char> > preparedData = getStringPrepared(s, profile);
	if (preparedData.empty()) {
		throw std::exception();
	}
	return createSafeByteArray(reinterpret_cast<const char*>(vecptr(preparedData)));
}

}

////////////////////////////////////////////////////////////////////////////////

#elif defined(HAVE_LIBIDN)

namespace {
	static const int MAX_STRINGPREP_SIZE = 1024;

	const Stringprep_profile* getLibIDNProfile(StringPrep::Profile profile) {
		switch(profile) {
			case StringPrep::NamePrep: return stringprep_nameprep; break;
			case StringPrep::XMPPNodePrep: return stringprep_xmpp_nodeprep; break;
			case StringPrep::XMPPResourcePrep: return stringprep_xmpp_resourceprep; break;
			case StringPrep::SASLPrep: return stringprep_saslprep; break;
		}
		assert(false);
		return 0;
	}

	template<typename StringType, typename ContainerType>
	ContainerType getStringPrepared(const StringType& s, StringPrep::Profile profile) {
		ContainerType input(s.begin(), s.end());
		input.resize(MAX_STRINGPREP_SIZE);
		if (stringprep(&input[0], MAX_STRINGPREP_SIZE, static_cast<Stringprep_profile_flags>(0), getLibIDNProfile(profile)) == 0) {
			return input;
		}
		else {
			return ContainerType();
		}
	}
}

namespace Swift {

std::string StringPrep::getPrepared(const std::string& s, Profile profile) {
	std::vector<char> preparedData = getStringPrepared< std::string, std::vector<char> >(s, profile);
	if (preparedData.empty()) {
		throw std::exception();
	}
	return std::string(vecptr(preparedData));
}

SafeByteArray StringPrep::getPrepared(const SafeByteArray& s, Profile profile) {
	std::vector<char, SafeAllocator<char> > preparedData = getStringPrepared<SafeByteArray, std::vector<char, SafeAllocator<char> > >(s, profile);
	if (preparedData.empty()) {
		throw std::exception();
	}
	return createSafeByteArray(reinterpret_cast<const char*>(vecptr(preparedData)));
}

}

#endif

