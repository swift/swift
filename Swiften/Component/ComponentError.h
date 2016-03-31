/*
 * Copyright (c) 2010-2015 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <Swiften/Base/API.h>

namespace Swift {
    class SWIFTEN_API ComponentError {
        public:
            enum Type {
                UnknownError,
                ConnectionError,
                ConnectionReadError,
                ConnectionWriteError,
                XMLError,
                AuthenticationFailedError,
                UnexpectedElementError
            };

            ComponentError(Type type = UnknownError) : type_(type) {}

            Type getType() const { return type_; }

        private:
            Type type_;
    };
}
