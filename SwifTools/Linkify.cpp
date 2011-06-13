/*
 * Copyright (c) 2010 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#include <SwifTools/Linkify.h>

#include <boost/regex.hpp>
#include <sstream>
#include <iostream>

namespace Swift {

static boost::regex linkifyRegexp("^(https?://|xmpp:).*");

std::string Linkify::linkify(const std::string& input) {
	std::ostringstream result;
	std::vector<char> currentURL;
	bool inURL = false;
	for (size_t i = 0; i < input.size(); ++i) {
		char c = input[i];
		if (inURL) {
			if (c != ' ' && c != '\t' && c != '\n') {
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

}
