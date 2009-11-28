#pragma once

#include <boost/signals.hpp>
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
