/*
 * Copyright (c) 2011-2016 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
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
