/*
 * Copyright (c) 2010 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <boost/shared_ptr.hpp>

namespace Swift {
	class UIEvent {
		public:
			typedef boost::shared_ptr<UIEvent> ref;

			virtual ~UIEvent();
	};
}
