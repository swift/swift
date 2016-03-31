/*
 * Copyright (c) 2010-2016 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <string>
#include <vector>

#include <Swiften/Base/boost_bsignals.h>
#include <Swiften/JID/JID.h>

namespace Swift {
    class JoinMUCWindow {
        public:
            virtual ~JoinMUCWindow() {}

            virtual void setNick(const std::string& nick) = 0;
            virtual void setMUC(const std::string& nick) = 0;
            virtual void show() = 0;

            boost::signal<void ()> onSearchMUC;
    };
}
