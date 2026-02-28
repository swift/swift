/*
 * Copyright (c) 2010-2018 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#include <Swiften/Base/URL.h>

#include <algorithm>
#include <iostream>

namespace Swift {

unsigned short URL::getPortOrDefaultPort(const URL& url) {
    if (url.getPort()) {
        return *url.getPort();
    }
    else if (url.getScheme() == "http") {
        return 80;
    }
    else if (url.getScheme() == "https") {
        return 443;
    }
    else {
        std::cerr << "Unknown scheme: " + url.getScheme() << std::endl;
        return 80;
    }
}

URL URL::fromString(const std::string& urlString) {
    size_t colonIndex = urlString.find(':');
    if (colonIndex == std::string::npos) {
        return URL();
    }
    std::string scheme = urlString.substr(0, colonIndex);

    // Authority
    if (urlString.size() > colonIndex + 2 && urlString[colonIndex+1] == '/' && urlString[colonIndex+2] == '/') {
        size_t authorityIndex = colonIndex + 3;
        size_t slashIndex = urlString.find('/', authorityIndex);
        std::string authority;
        std::string path;
        if (slashIndex == std::string::npos) {
            authority = urlString.substr(authorityIndex);
            path = "";
        }
        else {
            authority = urlString.substr(authorityIndex, slashIndex - authorityIndex);
            path = unescape(urlString.substr(slashIndex));
        }

        size_t atIndex = authority.find('@');
        std::string userInfo;
        std::string hostAndPort;
        if (atIndex != std::string::npos) {
            userInfo = authority.substr(0, atIndex);
            hostAndPort = authority.substr(atIndex + 1);
        }
        else {
            userInfo = "";
            hostAndPort = authority;
        }

        std::string host;
        boost::optional<unsigned short> port;
        if (hostAndPort[0] == '[') {
            // handle IPv6 address literals
            size_t addressEndIndex = hostAndPort.find(']');
            if (addressEndIndex != std::string::npos) {
                host = hostAndPort.substr(1, addressEndIndex - 1);
                colonIndex = hostAndPort.find(':', addressEndIndex);
                if (colonIndex != std::string::npos) {
                    try {
                        port = boost::numeric_cast<unsigned short>(boost::lexical_cast<int>(hostAndPort.substr(colonIndex + 1)));
                    }
                    catch (...) {
                        return URL();
                    }
                }
            }
            else {
                return URL();
            }
        }
        else {
            colonIndex = hostAndPort.find(':');
            if (colonIndex != std::string::npos) {
                host = unescape(hostAndPort.substr(0, colonIndex));
                try {
                    port = boost::numeric_cast<unsigned short>(boost::lexical_cast<int>(hostAndPort.substr(colonIndex + 1)));
                }
                catch (const boost::bad_lexical_cast&) {
                    return URL();
                }
            }
            else {
                host = unescape(hostAndPort);
            }
        }

        if (port) {
            return URL(scheme, host, *port, path);
        }
        else {
            return URL(scheme, host, path);
        }
    }
    else {
        // We don't support URLs without authorities yet
        return URL();
    }
}

// FIXME: Escape non-ascii characters
std::string URL::toString() const {
    if (empty) {
        return "";
    }
    std::string result = scheme + "://";
    if (!user.empty()) {
        result += user;
        if (!password.empty()) {
            result += ":" + password;
        }
        result += "@";
    }
    if (host.find(':') != std::string::npos) {
        result += "[" + host + "]";
    }
    else {
        result += host;
    }
    if (port) {
        result += ":";
        result += std::to_string(*port);
    }
    result += path;
    return result;
}

// Disabling this code for now, since GCC4.5+boost1.42 (on ubuntu) seems to
// result in a bug. Replacing it with naive code.
#if 0
// Should be in anonymous namespace, but older GCCs complain if we do that
struct PercentEncodedCharacterFinder {
template<typename Iterator>
boost::iterator_range<Iterator> operator()(Iterator begin, Iterator end) {
    boost::iterator_range<Iterator> r = boost::first_finder("%")(begin, end);
    if (r.end() == end) {
        return r;
    }
    else {
        if (r.end() + 1 == end || r.end() + 2 == end) {
            throw std::runtime_error("Incomplete escape character");
        }
        else {
            r.advance_end(2);
            return r;
        }
    }
}
};

struct PercentUnencodeFormatter {
template<typename FindResult>
std::string operator()(const FindResult& match) const {
    std::stringstream s;
    s << std::hex << std::string(match.begin() + 1, match.end());
    unsigned int value;
    s >> value;
    if (s.fail() || s.bad()) {
        throw std::runtime_error("Invalid escape character");
    }
    unsigned char charValue = static_cast<unsigned char>(value);
    return std::string(reinterpret_cast<const char*>(&charValue), 1);
}
};

std::string unescape(const std::string& s) {
    try {
        return boost::find_format_all_copy(s, PercentEncodedCharacterFinder(), PercentUnencodeFormatter());
    }
    catch (const std::exception&) {
        return "";
    }
}
#endif

std::string URL::unescape(const std::string& str) {
    std::string result;
    for (size_t i = 0; i < str.size(); ++i) {
        if (str[i] == '%') {
            if (i + 3 < str.size()) {
                std::stringstream s;
                s << std::hex << str.substr(i+1, 2);
                unsigned int value;
                s >> value;
                if (s.fail() || s.bad()) {
                    return "";
                }
                unsigned char charValue = static_cast<unsigned char>(value);
                result += std::string(reinterpret_cast<const char*>(&charValue), 1);
                i += 2;
            }
            else {
                return "";
            }
        }
        else {
            result += str[i];
        }
    }
    return result;
}

}
