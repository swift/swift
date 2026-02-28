/*
 * Copyright (c) 2010-2016 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <set>

#include <Swift/Controllers/UIInterfaces/UserSearchWindow.h>

namespace Swift {
    class UIEventStream;
    class UserSearchWindowFactory {
        public:
            virtual ~UserSearchWindowFactory() {}

            virtual UserSearchWindow* createUserSearchWindow(UserSearchWindow::Type type, UIEventStream* eventStream, const std::set<std::string>& groups) = 0;
    };
}
