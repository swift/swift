/*
 * Copyright (c) 2010-2016 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <string>

#include <Swiften/Base/API.h>
#include <Swiften/Elements/CapsInfo.h>

namespace Swift {
    class DiscoInfo;
    class CryptoProvider;

    class SWIFTEN_API CapsInfoGenerator {
        public:
            CapsInfoGenerator(const std::string& node, CryptoProvider* crypto);

            CapsInfo generateCapsInfo(const DiscoInfo& discoInfo) const;

        private:
            std::string node_;
            CryptoProvider* crypto_;
    };
}
