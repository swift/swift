/*
 * Copyright (c) 2010 Kevin Smith
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#pragma once

#include "Swift/Controllers/UIEvents/UIEvent.h"

namespace Swift {
	class ToggleSoundsUIEvent : public UIEvent {
		public:
			ToggleSoundsUIEvent(bool enable) : enabled_(enable) {};
			bool getEnabled() {return enabled_;};
		private:
			bool enabled_;
	};
}
