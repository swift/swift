/*
 * Copyright (c) 2010-2014 Kevin Smith and Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#include <Swiften/Base/FileSize.h>
#include <boost/format.hpp>

namespace Swift {

std::string formatSize(const boost::uintmax_t bytes) {
	static const char *siPrefix[] = {"k", "M", "G", "T", "P", "E", "Z", "Y", NULL};
	int power = 0;
	double engBytes = bytes;
	while (engBytes >= 1000) {
		++power;
		engBytes = engBytes / 1000.0;
	}
	return str( boost::format("%.1lf %sB") % engBytes % (power > 0 ? siPrefix[power-1] : "") );
}

}
