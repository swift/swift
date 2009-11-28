#pragma once

#include <boost/signals.hpp>
#include <boost/bind.hpp>
#include <boost/shared_ptr.hpp>

#include "Swift/Controllers/UIEvents/UIEventStream.h"

namespace Swift {
	class String;
	class XMLConsoleWidgetFactory;
	class XMLConsoleWidget;

	class XMLConsoleController {
		public:
			XMLConsoleController(UIEventStream* uiEventStream, XMLConsoleWidgetFactory* xmlConsoleWidgetFactory);
			~XMLConsoleController();

		public:
			void handleDataRead(const String& data);
			void handleDataWritten(const String& data);

		private:
			void handleUIEvent(boost::shared_ptr<UIEvent> event);

		private:
			XMLConsoleWidgetFactory* xmlConsoleWidgetFactory;
			XMLConsoleWidget* xmlConsoleWidget;
	};
}
