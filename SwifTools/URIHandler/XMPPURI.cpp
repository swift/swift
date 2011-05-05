/*
 * Copyright (c) 2011 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#include <SwifTools/URIHandler/XMPPURI.h>

#include <boost/algorithm/string/predicate.hpp>
#include <boost/algorithm/string/find_format.hpp>
#include <boost/algorithm/string/formatter.hpp>
#include <boost/algorithm/string/find_iterator.hpp>
#include <boost/algorithm/string/split.hpp>
#include <boost/algorithm/string/classification.hpp>
#include <sstream>
#include <stdexcept>
#include <vector>

using namespace Swift;

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

namespace {
	std::string unescape(const std::string& s) {
		try {
			return boost::find_format_all_copy(s, PercentEncodedCharacterFinder(), PercentUnencodeFormatter());
		}
		catch (const std::exception&) {
			return "";
		}
	}
}
#endif
namespace {
	std::string unescape(const std::string& str) {
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
			result.setAuthority(JID(unescape(uri.substr(2, i - 2))));
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
			result.setPath(JID(unescape(uri.substr(0, i))));
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
	    			result.addQueryParameter(unescape(keyValue[0]), "");
	    		}
	    		else if (keyValue.size() >= 2) {
	    			result.addQueryParameter(unescape(keyValue[0]), unescape(keyValue[1]));
	    		}
	    	}
	    	else {
	    		result.setQueryType(unescape(boost::copy_range<std::string>(*it)));
	    		haveType = true;
	    	}
	    }
	    uri = (end == uri.npos ? "" : uri.substr(end + 1, uri.npos));
		}

		// Parse fragment
		if (parseFragment) {
			result.setFragment(unescape(uri));
		}
	}
	return result;
}
