/*
 * Copyright (c) 2011 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#pragma once

#include <string>
#include <Swiften/Base/boost_bsignals.h>

namespace Swift {
	class URIHandler {
		public:
			URIHandler();
			virtual ~URIHandler();

			boost::signal<void (const std::string&)> onURI;
	};
}
