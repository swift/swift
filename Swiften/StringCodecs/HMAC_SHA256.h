/*
 * Copyright (c) 2011 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#pragma once

#include <Swiften/StringCodecs/HMAC.h>
#include <Swiften/StringCodecs/SHA256.h>

namespace Swift {
	typedef HMAC<SHA256, 64> HMAC_SHA256;
}
