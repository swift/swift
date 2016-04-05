/*
 * Copyright (c) 2010-2016 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <string>

#include <boost/signals2.hpp>

#include <Swiften/Base/API.h>

namespace Swift {
    class SWIFTEN_API NickManager {
        public:
            virtual ~NickManager();

            virtual std::string getOwnNick() const = 0;
            virtual void setOwnNick(const std::string& nick) = 0;

            boost::signals2::signal<void (const std::string&)> onOwnNickChanged;
    };
}
