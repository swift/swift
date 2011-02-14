/*
 * Copyright (c) 2010 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#pragma once

#include <boost/optional.hpp>
#include <boost/shared_ptr.hpp>

#include <string>
#include <Swift/Controllers/UIEvents/UIEvent.h>

namespace Swift {
	class RequestJoinMUCUIEvent : public UIEvent {
		public:
			typedef boost::shared_ptr<RequestJoinMUCUIEvent> ref;

			RequestJoinMUCUIEvent() {
			}
	};
}
