/*
 * Copyright (c) 2010 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#include "SwifTools/Linkify.h"

#include <boost/regex.hpp>

namespace Swift {

static const boost::regex linkifyRegexp("(https?://([@:\\-\\w\\.]+)+(:\\d+)?(/([=%~\\-\\w/_#\\.\\+]*(\\?\\S+)?)?)?)");

String Linkify::linkify(const String& input) {
	return String(boost::regex_replace(
			input.getUTF8String(), 
			linkifyRegexp, 
			"<a href=\"\\1\">\\1</a>", 
			boost::match_default|boost::format_all));
}

}
