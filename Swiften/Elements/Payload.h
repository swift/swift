/*
 * Copyright (c) 2010 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#pragma once

#include <boost/shared_ptr.hpp>

#include <Swiften/Base/API.h>

namespace Swift {
	class SWIFTEN_API Payload {
		public:
			typedef boost::shared_ptr<Payload> ref;
		public:
			virtual ~Payload();
	};
}
