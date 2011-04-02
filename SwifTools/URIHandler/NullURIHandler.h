/*
 * Copyright (c) 2011 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#pragma once

#include <SwifTools/URIHandler/URIHandler.h>

namespace Swift {
	class NullURIHandler : public URIHandler {
		public:
			virtual void start() {
			}

			virtual void stop() {
			}
	};
}
