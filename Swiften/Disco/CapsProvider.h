/*
 * Copyright (c) 2010-2016 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <boost/signals2.hpp>

#include <Swiften/Base/API.h>
#include <Swiften/Elements/CapsInfo.h>
#include <Swiften/Elements/DiscoInfo.h>

namespace Swift {


    class SWIFTEN_API CapsProvider {
        public:
            virtual ~CapsProvider() {}

            virtual DiscoInfo::ref getCaps(const std::string&) const = 0;

            boost::signals2::signal<void (const std::string&)> onCapsAvailable;
    };
}
