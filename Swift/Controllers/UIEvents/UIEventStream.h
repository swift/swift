/*
 * Copyright (c) 2010 Kevin Smith
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
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
			};
	};
}
