/*
 * Copyright (c) 2010 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#include <Swiften/IDN/IDNA.h>

#include <stringprep.h>
#include <vector>
#include <cstdlib>
#include <idna.h>

namespace Swift {

std::string IDNA::getEncoded(const std::string& domain) {
	char* output;
	if (idna_to_ascii_8z(domain.c_str(), &output, 0) == IDNA_SUCCESS) {
		std::string result(output);
		free(output);
		return result;
	}
	else {
		return domain;
	}
}

}
