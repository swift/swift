#pragma once

#include "Swift/Controllers/UIInterfaces/XMLConsoleWidgetFactory.h"

#include "QtXMLConsoleWidget.h"

namespace Swift {
	class QtChatTabs;
	class QtXMLConsoleWidgetFactory : public XMLConsoleWidgetFactory {
		public:
			QtXMLConsoleWidgetFactory(QtChatTabs* tabs);
			XMLConsoleWidget* createXMLConsoleWidget();
		private:
			QtChatTabs* tabs_;
	};
}
