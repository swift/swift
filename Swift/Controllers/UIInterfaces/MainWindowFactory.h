/*
 * Copyright (c) 2010-2018 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

namespace Swift {
    class Chattables;
    class MainWindow;
    class UIEventStream;

    class MainWindowFactory {
        public:
            virtual ~MainWindowFactory() {}
            /**
             * Transfers ownership of result.
             */
            virtual MainWindow* createMainWindow(Chattables&, UIEventStream*) = 0;

    };
}
