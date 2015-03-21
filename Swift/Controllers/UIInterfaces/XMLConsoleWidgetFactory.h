/*
 * Copyright (c) 2010 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include "Swift/Controllers/UIInterfaces/XMLConsoleWidget.h"

namespace Swift {
	class UIEventStream;
	class XMLConsoleController;
	class XMLConsoleWidgetFactory {
		public:
			virtual ~XMLConsoleWidgetFactory() {}

			virtual XMLConsoleWidget* createXMLConsoleWidget(UIEventStream*) = 0;
			virtual XMLConsoleWidget* createXMLConsoleMsgWidget(XMLConsoleController*) = 0 ;
	};
}
