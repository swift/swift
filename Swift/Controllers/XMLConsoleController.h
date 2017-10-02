/*
 * Copyright (c) 2010-2016 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <memory>

#include <boost/bind.hpp>
#include <boost/signals2.hpp>

#include <Swiften/Base/SafeByteArray.h>

#include <Swift/Controllers/UIEvents/UIEventStream.h>

namespace Swift {

    class XMLConsoleWidgetFactory;
    class XMLConsoleWidget;

    class XMLConsoleController {
        public:
            XMLConsoleController(UIEventStream* uiEventStream, XMLConsoleWidgetFactory* xmlConsoleWidgetFactory);
            ~XMLConsoleController();

        public:
            void handleDataRead(const SafeByteArray& data);
            void handleDataWritten(const SafeByteArray& data);

        private:
            void handleUIEvent(std::shared_ptr<UIEvent> event);

        private:
            XMLConsoleWidgetFactory* xmlConsoleWidgetFactory;
            XMLConsoleWidget* xmlConsoleWidget;
    };
}
