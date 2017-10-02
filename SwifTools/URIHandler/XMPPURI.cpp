/*
 * Copyright (c) 2011-2016 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#include <SwifTools/URIHandler/XMPPURI.h>

#include <sstream>
#include <stdexcept>
#include <vector>

#include <boost/algorithm/string/classification.hpp>
#include <boost/algorithm/string/find_format.hpp>
#include <boost/algorithm/string/find_iterator.hpp>
#include <boost/algorithm/string/formatter.hpp>
#include <boost/algorithm/string/predicate.hpp>
#include <boost/algorithm/string/split.hpp>

#include <Swiften/Base/URL.h>

using namespace Swift;


XMPPURI::XMPPURI() {
}

XMPPURI XMPPURI::fromString(const std::string& s) {
    XMPPURI result;
    if (boost::starts_with(s, "xmpp:")) {
        std::string uri = s.substr(5, s.npos);
        bool parsePath = true;
        bool parseQuery = true;
        bool parseFragment = true;

        // Parse authority
        if (boost::starts_with(uri, "//")) {
            size_t i = uri.find_first_of("/#?", 2);
            result.setAuthority(JID(URL::unescape(uri.substr(2, i - 2))));
            if (i == uri.npos) {
                uri = "";
                parsePath = parseQuery = parseFragment = false;
            }
            else {
                if (uri[i] == '?') {
                    parsePath = false;
                }
                else if (uri[i] == '#') {
                    parseQuery = parsePath = false;
                }
                uri = uri.substr(i + 1, uri.npos);
            }
        }

        // Parse path
        if (parsePath) {
            size_t i = uri.find_first_of("#?");
            result.setPath(JID(URL::unescape(uri.substr(0, i))));
            if (i == uri.npos) {
                uri = "";
                parseQuery = parseFragment = false;
            }
            else {
                if (uri[i] == '#') {
                    parseQuery = false;
                }
                uri = uri.substr(i + 1, uri.npos);
            }
        }

        // Parse query
        if (parseQuery) {
            size_t end = uri.find_first_of("#");
            std::string query = uri.substr(0, end);
            bool haveType = false;
            typedef boost::split_iterator<std::string::iterator> split_iterator;
        for (split_iterator it = boost::make_split_iterator(query, boost::first_finder(";")); it != split_iterator(); ++it) {
            if (haveType) {
                std::vector<std::string> keyValue;
                boost::split(keyValue, *it, boost::is_any_of("="));
                if (keyValue.size() == 1) {
                    result.addQueryParameter(URL::unescape(keyValue[0]), "");
                }
                else if (keyValue.size() >= 2) {
                    result.addQueryParameter(URL::unescape(keyValue[0]), URL::unescape(keyValue[1]));
                }
            }
            else {
                result.setQueryType(URL::unescape(boost::copy_range<std::string>(*it)));
                haveType = true;
            }
        }
        uri = (end == uri.npos ? "" : uri.substr(end + 1, uri.npos));
        }

        // Parse fragment
        if (parseFragment) {
            result.setFragment(URL::unescape(uri));
        }
    }
    return result;
}
