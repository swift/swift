/*
 * Copyright (c) 2010-2016 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <Swift/Controllers/UIInterfaces/MainWindowFactory.h>
#include <Swift/Controllers/UnitTest/MockMainWindow.h>

namespace Swift {

    class MockMainWindowFactory : public MainWindowFactory {
        public:
            MockMainWindowFactory() : last(nullptr) {}

            virtual ~MockMainWindowFactory() {}

            /**
             * Transfers ownership of result.
             */
            virtual MainWindow* createMainWindow(Chattables&, UIEventStream*) {last = new MockMainWindow();return last;}
            MockMainWindow* last;
    };
}
