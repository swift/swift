/*
 * Copyright (c) 2013 Remko Tronçon
 * Licensed under the GNU General Public License.
 * See the COPYING file for more information.
 */

#pragma once

#include <Swiften/Base/API.h>

namespace Swift {
	class CryptoProvider;

	namespace PlatformCryptoProvider {
		SWIFTEN_API CryptoProvider* create();
	}
}
