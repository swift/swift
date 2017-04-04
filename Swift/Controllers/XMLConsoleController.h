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
#include <Swiften/Client/Client.h>

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
            void setClient(std::shared_ptr<Client> client_);

        private:
            void handleUIEvent(std::shared_ptr<UIEvent> event);
            void sendXML(std::string data);
        private:
            XMLConsoleWidgetFactory* xmlConsoleWidgetFactory;
            XMLConsoleWidget* xmlConsoleWidget;
            std::shared_ptr<Client> client_;
    };
}
