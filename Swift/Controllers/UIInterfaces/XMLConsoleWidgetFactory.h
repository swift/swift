#pragma once

#include "Swift/Controllers/UIInterfaces/XMLConsoleWidget.h"

namespace Swift {
	class UIEventStream;
	class XMLConsoleWidgetFactory {
		public:
			virtual XMLConsoleWidget* createXMLConsoleWidget() = 0;
			virtual ~XMLConsoleWidgetFactory() {};
	};
}
