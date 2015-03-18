/*
 * Copyright (c) 2010 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include "Swiften/Base/boost_bsignals.h"
#include <boost/shared_ptr.hpp>

#include "Swift/Controllers/UIEvents/UIEvent.h"

namespace Swift {
	class UIEventStream {
		public:
			boost::signal<void (boost::shared_ptr<UIEvent>)> onUIEvent;
			void send(boost::shared_ptr<UIEvent> event) {
				onUIEvent(event);
			}
	};
}
