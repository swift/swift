/*
 * Copyright (c) 2010 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#pragma once

#include <Swiften/StringCodecs/HMAC.h>
#include <Swiften/StringCodecs/SHA1.h>

namespace Swift {
	typedef HMAC<SHA1, 64> HMAC_SHA1;
}
