/*
 * Copyright (c) 2010 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#pragma once

#include <string>

#include <Swiften/Base/API.h>

namespace Swift {
	class SWIFTEN_API XMLNode {
		public:
			virtual ~XMLNode();

			virtual std::string serialize() = 0;
	};
}
