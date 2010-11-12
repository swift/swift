/*
 * Copyright (c) 2010 Kevin Smith
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#pragma once

#include "Swift/Controllers/UIEvents/UIEvent.h"

namespace Swift {
	class ToggleShowOfflineUIEvent : public UIEvent {
		public:
			ToggleShowOfflineUIEvent(bool show) : show_(show) {};
			bool getShow() {return show_;};
		private:
			bool show_;
	};
}
