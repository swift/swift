/*
 * Copyright (c) 2012-2018 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#include <Swiften/IDN/ICUConverter.h>

#pragma GCC diagnostic ignored "-Wold-style-cast"
#pragma clang diagnostic ignored "-Wheader-hygiene"
#include <unicode/uidna.h>
#include <unicode/usprep.h>
#include <unicode/ucnv.h>
#include <unicode/ustring.h>

 #include <boost/numeric/conversion/cast.hpp>

using namespace Swift;
using boost::numeric_cast;

namespace {
    static constexpr auto maxStringPrepLength = 1023;

    typedef std::vector<UChar, SafeAllocator<UChar> > ICUString;

    const char* toConstCharArray(const std::string& input) {
        return input.c_str();
    }

    const char* toConstCharArray(const std::vector<unsigned char, SafeAllocator<unsigned char> >& input) {
        return reinterpret_cast<const char*>(vecptr(input));
    }

    template<typename T>
    ICUString convertToICUString(const T& s) {
        ICUString result;
        result.resize(s.size());
        UErrorCode status = U_ZERO_ERROR;
        int32_t icuResultLength = numeric_cast<int32_t>(result.size());
        u_strFromUTF8Lenient(vecptr(result), numeric_cast<int32_t>(result.size()), &icuResultLength, toConstCharArray(s), numeric_cast<int32_t>(s.size()), &status);
        if (status == U_BUFFER_OVERFLOW_ERROR) {
            status = U_ZERO_ERROR;
            result.resize(numeric_cast<size_t>(icuResultLength));
            u_strFromUTF8Lenient(vecptr(result), numeric_cast<int32_t>(result.size()), &icuResultLength, toConstCharArray(s), numeric_cast<int32_t>(s.size()), &status);
        }
        if (U_FAILURE(status)) {
            return ICUString();
        }
        result.resize(numeric_cast<size_t>(icuResultLength));
        return result;
    }

    std::vector<char, SafeAllocator<char> > convertToArray(const ICUString& input) {
        std::vector<char, SafeAllocator<char> > result;
        result.resize(input.size());
        UErrorCode status = U_ZERO_ERROR;
        int32_t inputLength = numeric_cast<int32_t>(result.size());
        u_strToUTF8(vecptr(result), numeric_cast<int32_t>(result.size()), &inputLength, vecptr(input), numeric_cast<int32_t>(input.size()), &status);
        if (status == U_BUFFER_OVERFLOW_ERROR) {
            status = U_ZERO_ERROR;
            result.resize(numeric_cast<size_t>(inputLength));
            u_strToUTF8(vecptr(result), numeric_cast<int32_t>(result.size()), &inputLength, vecptr(input), numeric_cast<int32_t>(input.size()), &status);
        }
        if (U_FAILURE(status)) {
            return std::vector<char, SafeAllocator<char> >();
        }

        result.resize(numeric_cast<size_t>(inputLength) + 1);
        result[result.size() - 1] = '\0';
        return result;
    }

    std::string convertToString(const ICUString& input) {
        return std::string(vecptr(convertToArray(input)));
    }

    UStringPrepProfileType getICUProfileType(IDNConverter::StringPrepProfile profile) {
        switch(profile) {
            case IDNConverter::NamePrep: return USPREP_RFC3491_NAMEPREP;
            case IDNConverter::XMPPNodePrep: return USPREP_RFC3920_NODEPREP;
            case IDNConverter::XMPPResourcePrep: return USPREP_RFC3920_RESOURCEPREP;
            case IDNConverter::SASLPrep: return USPREP_RFC4013_SASLPREP;
        }
        assert(false);
        return USPREP_RFC3491_NAMEPREP;
    }

    template<typename StringType>
    std::vector<char, SafeAllocator<char> > getStringPreparedDetail(const StringType& s, IDNConverter::StringPrepProfile profile) {
        UErrorCode status = U_ZERO_ERROR;

        std::shared_ptr<UStringPrepProfile> icuProfile(usprep_openByType(getICUProfileType(profile), &status), usprep_close);
        assert(U_SUCCESS(status));

        ICUString icuInput = convertToICUString(s);
        ICUString icuResult;
        UParseError parseError;
        icuResult.resize(maxStringPrepLength);
        int32_t icuResultLength = usprep_prepare(icuProfile.get(), vecptr(icuInput), numeric_cast<int32_t>(icuInput.size()), vecptr(icuResult), numeric_cast<int32_t>(icuResult.size()), USPREP_ALLOW_UNASSIGNED, &parseError, &status);
        if (U_FAILURE(status)) {
            return std::vector<char, SafeAllocator<char> >();
        }
        icuResult.resize(numeric_cast<size_t>(icuResultLength));

        return convertToArray(icuResult);
    }
}

namespace Swift {

std::string ICUConverter::getStringPrepared(const std::string& s, StringPrepProfile profile) {
    if (s.empty()) {
        return "";
    }
    std::vector<char, SafeAllocator<char> > preparedData = getStringPreparedDetail(s, profile);
    if (preparedData.empty()) {
        throw std::exception();
    }
    return std::string(vecptr(preparedData));
}

SafeByteArray ICUConverter::getStringPrepared(const SafeByteArray& s, StringPrepProfile profile) {
    if (s.empty()) {
        return SafeByteArray();
    }
    std::vector<char, SafeAllocator<char> > preparedData = getStringPreparedDetail(s, profile);
    if (preparedData.empty()) {
        throw std::exception();
    }
    return createSafeByteArray(reinterpret_cast<const char*>(vecptr(preparedData)));
}

boost::optional<std::string> ICUConverter::getIDNAEncoded(const std::string& domain) {
    UErrorCode status = U_ZERO_ERROR;
    ICUString icuInput = convertToICUString(domain);
    ICUString icuResult;
    icuResult.resize(icuInput.size());
    UParseError parseError;
    int32_t icuResultLength = uidna_IDNToASCII(vecptr(icuInput), numeric_cast<int32_t>(icuInput.size()), vecptr(icuResult), numeric_cast<int32_t>(icuResult.size()), UIDNA_USE_STD3_RULES, &parseError, &status);
    if (status == U_BUFFER_OVERFLOW_ERROR) {
        status = U_ZERO_ERROR;
        icuResult.resize(numeric_cast<size_t>(icuResultLength));
        icuResultLength = uidna_IDNToASCII(vecptr(icuInput), numeric_cast<int32_t>(icuInput.size()), vecptr(icuResult), numeric_cast<int32_t>(icuResult.size()), UIDNA_USE_STD3_RULES, &parseError, &status);
    }
    if (U_FAILURE(status)) {
        return boost::optional<std::string>();
    }
    icuResult.resize(numeric_cast<size_t>(icuResultLength));
    return convertToString(icuResult);
}

}
