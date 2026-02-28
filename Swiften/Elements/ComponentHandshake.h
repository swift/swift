/*
 * Copyright (c) 2010-2016 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <memory>
#include <string>

#include <Swiften/Base/API.h>
#include <Swiften/Elements/ToplevelElement.h>

namespace Swift {
    class SWIFTEN_API ComponentHandshake : public ToplevelElement {
        public:
            typedef std::shared_ptr<ComponentHandshake> ref;

            ComponentHandshake(const std::string& data = "") : data(data) {
            }

            void setData(const std::string& d) {
                data = d;
            }

            const std::string& getData() const {
                return data;
            }

        private:
            std::string data;
    };
}
