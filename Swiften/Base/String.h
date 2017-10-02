/*
 * Copyright (c) 2010-2016 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <cstdint>
#include <sstream>
#include <string>
#include <vector>

#include <Swiften/Base/API.h>
#include <Swiften/Base/Platform.h>
#define SWIFTEN_STRING_TO_CFSTRING(a) \
    CFStringCreateWithBytes(NULL, reinterpret_cast<const UInt8*>(a.c_str()), a.size(), kCFStringEncodingUTF8, false)

namespace Swift {
    namespace String {
            SWIFTEN_API std::vector<unsigned int> getUnicodeCodePoints(const std::string&);
            SWIFTEN_API std::pair<std::string, std::string> getSplittedAtFirst(const std::string&, char c);
            SWIFTEN_API std::vector<std::string> split(const std::string&, char c);
            SWIFTEN_API void replaceAll(std::string&, char c, const std::string& s);
            SWIFTEN_API bool isValidXMPPCharacter(std::uint32_t codepoint);
            SWIFTEN_API std::string sanitizeXMPPString(const std::string& input);

            inline bool beginsWith(const std::string& s, char c) {
                return s.size() > 0 && s[0] == c;
            }

            inline bool endsWith(const std::string& s, char c) {
                return s.size() > 0 && s[s.size()-1] == c;
            }

            std::string convertIntToHexString(int h);
            int convertHexStringToInt(const std::string& s);

    }

#ifdef SWIFTEN_PLATFORM_WINDOWS
    SWIFTEN_API std::string convertWStringToString(const std::wstring& s);
    SWIFTEN_API std::wstring convertStringToWString(const std::string& s);
#endif

    class SWIFTEN_API makeString {
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
