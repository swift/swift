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
