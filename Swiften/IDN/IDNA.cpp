/*
 * Copyright (c) 2010 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#include "Swiften/IDN/IDNA.h"

#include <stringprep.h>
#include <vector>
#include <idna.h>

namespace Swift {

String IDNA::getEncoded(const String& domain) {
	char* output;
	if (idna_to_ascii_8z(domain.getUTF8Data(), &output, 0) == IDNA_SUCCESS) {
		String result(output);
		free(output);
		return result;
	}
	else {
		return domain;
	}
}

}
