/*
 * Copyright (c) 2012 Isode Limited, London, England.
 * Licensed under the simplified BSD license.
 * See Documentation/Licenses/BSD-simplified.txt for more information.
 */

#pragma once

#include <string>

namespace Swift {
	std::string selectCAPICertificate();
	bool isCAPIURI(std::string uri);
}
