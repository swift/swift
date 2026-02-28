/*
 * Copyright (c) 2010 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#ifndef SWIFTEN_LoginWindowFactory_H
#define SWIFTEN_LoginWindowFactory_H



namespace Swift {
    class LoginWindow;

    class UIEventStream;

    class LoginWindowFactory {
        public:
            virtual ~LoginWindowFactory() {}

            /**
             * Transfers ownership of result.
             */
            virtual LoginWindow* createLoginWindow(UIEventStream* uiEventStream) = 0;

    };
}
#endif

