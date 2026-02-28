/*
 * Copyright (c) 2012-2018 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#include <Swiften/IDN/LibIDNConverter.h>

extern "C" {
    #include <stringprep.h>
    #include <idna.h>
}

#include <cassert>
#include <cstdlib>
#include <vector>

#include <memory>

#include <Swiften/Base/ByteArray.h>
#include <Swiften/Base/SafeAllocator.h>
#include <Swiften/IDN/UTF8Validator.h>

using namespace Swift;

namespace {
    static const size_t MAX_STRINGPREP_SIZE = 1024;

    const Stringprep_profile* getLibIDNProfile(IDNConverter::StringPrepProfile profile) {
        switch(profile) {
            case IDNConverter::NamePrep: return stringprep_nameprep;
            case IDNConverter::XMPPNodePrep: return stringprep_xmpp_nodeprep;
            case IDNConverter::XMPPResourcePrep: return stringprep_xmpp_resourceprep;
            case IDNConverter::SASLPrep: return stringprep_saslprep;
        }
        assert(false);
        return nullptr;
    }

    template<typename StringType, typename ContainerType>
    ContainerType getStringPreparedInternal(const StringType& s, IDNConverter::StringPrepProfile profile) {
        ContainerType input(s.begin(), s.end());
        if (!UTF8IsValid(s.data(), s.size())) {
            return ContainerType();
        }

        // Ensure we have enough space for stringprepping, and that input is always NUL terminated
        input.resize(std::max(MAX_STRINGPREP_SIZE, input.size() + 1));
        if (stringprep(&input[0], MAX_STRINGPREP_SIZE, static_cast<Stringprep_profile_flags>(0), getLibIDNProfile(profile)) == 0) {
            return input;
        }
        else {
            return ContainerType();
        }
    }
}

namespace Swift {

std::string LibIDNConverter::getStringPrepared(const std::string& s, StringPrepProfile profile) {
    std::vector<char> preparedData = getStringPreparedInternal< std::string, std::vector<char> >(s, profile);
    if (preparedData.empty()) {
        throw std::exception();
    }
    return std::string(vecptr(preparedData));
}

SafeByteArray LibIDNConverter::getStringPrepared(const SafeByteArray& s, StringPrepProfile profile) {
    std::vector<char, SafeAllocator<char> > preparedData = getStringPreparedInternal<SafeByteArray, std::vector<char, SafeAllocator<char> > >(s, profile);
    if (preparedData.empty()) {
        throw std::exception();
    }
    return createSafeByteArray(reinterpret_cast<const char*>(vecptr(preparedData)));
}

boost::optional<std::string> LibIDNConverter::getIDNAEncoded(const std::string& domain) {
    char* output;
    if (idna_to_ascii_8z(domain.c_str(), &output, IDNA_USE_STD3_ASCII_RULES) == IDNA_SUCCESS) {
        std::string result(output);
        free(output);
        if (result.size() > 255) {
            return boost::optional<std::string>();
        }
        return result;
    }
    else {
        return boost::optional<std::string>();
    }
}

}
