#pragma once

#include <boost/signals.hpp>
#include <boost/bind.hpp>
#include <boost/shared_ptr.hpp>

#include "Swift/Controllers/UIEvents/UIEventStream.h"

namespace Swift {
	class XMLConsoleWidgetFactory;
	class XMLConsoleWidget;
	class XMLConsoleController {
		public:
			XMLConsoleController(UIEventStream* uiEventStream, XMLConsoleWidgetFactory* xmlConsoleWidgetFactory);
		private:
			void handleUIEvent(boost::shared_ptr<UIEvent> event);
			UIEventStream* uiEventStream_;
			XMLConsoleWidgetFactory* xmlConsoleWidgetFactory_;
			XMLConsoleWidget* xmlConsoleWidget_;
	};
}
