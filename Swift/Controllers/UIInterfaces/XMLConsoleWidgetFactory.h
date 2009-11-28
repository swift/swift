#pragma once

#include "Swift/Controllers/UIInterfaces/XMLConsoleWidget.h"

namespace Swift {
	class UIEventStream;
	class XMLConsoleWidgetFactory {
		public:
			virtual ~XMLConsoleWidgetFactory() {};

			virtual XMLConsoleWidget* createXMLConsoleWidget() = 0;
	};
}
