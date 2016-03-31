/*
 * Copyright (c) 2010-2016 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <vector>

#include <boost/optional.hpp>

#include <Swiften/Base/API.h>
#include <Swiften/Base/SafeByteArray.h>
#include <Swiften/Elements/ToplevelElement.h>

namespace Swift {
    class SWIFTEN_API AuthResponse : public ToplevelElement {
        public:
            AuthResponse() {
            }

            AuthResponse(const SafeByteArray& value) : value(value) {
            }

            AuthResponse(const boost::optional<SafeByteArray>& value) : value(value) {
            }

            const boost::optional<SafeByteArray>& getValue() const {
                return value;
            }

            void setValue(const SafeByteArray& value) {
                this->value = boost::optional<SafeByteArray>(value);
            }

        private:
            boost::optional<SafeByteArray> value;
    };
}
