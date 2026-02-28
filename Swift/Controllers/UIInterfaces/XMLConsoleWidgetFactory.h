/*
 * Copyright (c) 2010-2016 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <Swift/Controllers/UIInterfaces/XMLConsoleWidget.h>

namespace Swift {
    class UIEventStream;
    class XMLConsoleWidgetFactory {
        public:
            virtual ~XMLConsoleWidgetFactory() {}

            virtual XMLConsoleWidget* createXMLConsoleWidget() = 0;
    };
}
