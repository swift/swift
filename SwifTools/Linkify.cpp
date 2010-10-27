/*
 * Copyright (c) 2010 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#include "SwifTools/Linkify.h"

#include <boost/regex.hpp>
#include <sstream>
#include <iostream>

namespace Swift {

static boost::regex linkifyRegexp("^https?://.*");

String Linkify::linkify(const String& input) {
	std::ostringstream result;
	std::vector<char> currentURL;
	bool inURL = false;
	for (size_t i = 0; i < input.getUTF8Size(); ++i) {
		char c = input[i];
		if (inURL) {
			if (c >= 33 && c != '\\' && c != '"') {
				currentURL.push_back(c);
			}
			else {
				String url(&currentURL[0], currentURL.size());
				result << "<a href=\"" << url << "\">" <<  url << "</a>";
				currentURL.clear();
				inURL = false;
				result << c;
			}
		}
		else {
			if (boost::regex_match(input.getSubstring(i, 8).getUTF8String(), linkifyRegexp)) {
				currentURL.push_back(c);
				inURL = true;
			}
			else {
				result << c;
			}
		}
	}
	if (currentURL.size() > 0) {
		String url(&currentURL[0], currentURL.size());
		result << "<a href=\"" << url << "\">" <<  url << "</a>";
	}
	return String(result.str());
}

}
