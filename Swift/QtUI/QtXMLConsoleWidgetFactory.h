/*
 * Copyright (c) 2010 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

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
