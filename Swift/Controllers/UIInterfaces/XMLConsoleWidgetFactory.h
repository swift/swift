/*
 * Copyright (c) 2010 Kevin Smith
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

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
