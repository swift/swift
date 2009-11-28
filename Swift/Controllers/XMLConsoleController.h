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
			~XMLConsoleController();

		private:
			void handleUIEvent(boost::shared_ptr<UIEvent> event);

		private:
			XMLConsoleWidgetFactory* xmlConsoleWidgetFactory;
			XMLConsoleWidget* xmlConsoleWidget;
	};
}
