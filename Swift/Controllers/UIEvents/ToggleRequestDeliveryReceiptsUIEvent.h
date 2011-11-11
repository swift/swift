/*
 * Copyright (c) 2011 Tobias Markmann
 * Licensed under the BSD license.
 * See http://www.opensource.org/licenses/bsd-license.php for more information.
 */

#pragma once

#include "Swift/Controllers/UIEvents/UIEvent.h"

namespace Swift {
	class ToggleRequestDeliveryReceiptsUIEvent : public UIEvent {
		public:
			ToggleRequestDeliveryReceiptsUIEvent(bool enable) : enabled_(enable) {}
			bool getEnabled() {return enabled_;}
		private:
			bool enabled_;
	};
}
