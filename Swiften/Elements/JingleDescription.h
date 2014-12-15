/*
 * Copyright (c) 2011 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <boost/shared_ptr.hpp>

#include <Swiften/Elements/Payload.h>

namespace Swift {
	class JingleDescription : public Payload {
		public:
			typedef boost::shared_ptr<JingleDescription> ref;
	};
}
