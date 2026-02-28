/*
 * Copyright (c) 2010-2016 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <memory>

#include <Swiften/Base/API.h>
#include <Swiften/Elements/DiscoInfo.h>

namespace Swift {
    class SWIFTEN_API CapsStorage {
        public:
            virtual ~CapsStorage();

            virtual DiscoInfo::ref getDiscoInfo(const std::string&) const = 0;
            virtual void setDiscoInfo(const std::string&, DiscoInfo::ref) = 0;
    };
}
