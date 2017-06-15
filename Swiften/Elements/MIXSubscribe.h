/*
 * Copyright (c) 2017 Tarun Gupta
 * Licensed under the simplified BSD license.
 * See Documentation/Licenses/BSD-simplified.txt for more information.
 */

#pragma once

#include <memory>
#include <string>

#include <boost/optional.hpp>

#include <Swiften/Base/API.h>
#include <Swiften/Elements/Payload.h>

namespace Swift {
    class SWIFTEN_API MIXSubscribe : public Payload {

        public:
            using ref = std::shared_ptr<MIXSubscribe>;

        public:

            MIXSubscribe() {}

            const std::string& getNode() const {
                return node_;
            }

            void setNode(const std::string& node) {
                node_ = node;
            }

        private:
            std::string node_;
    };
}
