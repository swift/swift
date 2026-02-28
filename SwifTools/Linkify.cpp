/*
 * Copyright (c) 2010-2016 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#include <SwifTools/Linkify.h>

#include <iostream>
#include <sstream>

#include <boost/regex.hpp>

namespace Swift {

static boost::regex linkifyRegexp("^(https?://|xmpp:).*");

std::string Linkify::linkify(const std::string& input) {
    std::ostringstream result;
    std::vector<char> currentURL;
    bool inURL = false;
    for (size_t i = 0; i < input.size(); ++i) {
        char c = input[i];
        if (inURL) {
            if (c != ' ' && c != '\t' && c != '\n' && !(c == '*' && i == input.size() - 1 && input[0] == '*')) {
                currentURL.push_back(c);
            }
            else {
                std::string url(&currentURL[0], currentURL.size());
                result << "<a href=\"" << url << "\">" <<  url << "</a>";
                currentURL.clear();
                inURL = false;
                result << c;
            }
        }
        else {
            if (boost::regex_match(input.substr(i, 8), linkifyRegexp)) {
                currentURL.push_back(c);
                inURL = true;
            }
            else {
                result << c;
            }
        }
    }
    if (!currentURL.empty()) {
        std::string url(&currentURL[0], currentURL.size());
        result << "<a href=\"" << url << "\">" <<  url << "</a>";
    }
    return std::string(result.str());
}

std::pair<std::vector<std::string>, size_t> Linkify::splitLink(const std::string& input) {
    std::vector<std::string> result;
    std::pair<std::vector<std::string>, size_t> pair;
    std::vector<char> currentURL;
    bool inURL = false;
    size_t urlStartsAt = 0;
    for (size_t i = 0; i < input.size(); ++i) {
        char c = input[i];
        if (inURL) {
            if (c != ' ' && c != '\t' && c != '\n' && !(c == '*' && i == input.size() - 1 && input[0] == '*')) {
                // Keep parsing
            }
            else {
                std::string url(input.substr(urlStartsAt, i - urlStartsAt));
                result.push_back(url);
                inURL = false;
                size_t remaining = input.size() - i;
                if (remaining > 0) {
                    result.push_back(input.substr(i, remaining));
                }
                pair.first = result;
                pair.second = urlStartsAt == 0 ? 0 : 1;
                return pair;
            }
        }
        else {
            if (boost::regex_match(input.substr(i, 8), linkifyRegexp)) {
                urlStartsAt = i;
                inURL = true;
                if (i > 0) {
                    result.push_back(input.substr(0, i));
                }
            }
            else {
                // Just keep swimming
            }
        }
    }
    if (urlStartsAt > 0 || inURL) {
        std::string url(input.substr(urlStartsAt, input.size() - urlStartsAt));
        result.push_back(url);
        pair.first = result;
        pair.second = urlStartsAt == 0 ? 0 : 1;
    }
    else {
        pair.first.push_back(input);
        pair.second = 1;
    }
    return pair;
}

}
