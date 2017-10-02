/*
 * Copyright (c) 2012-2016 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <memory>

#include <Swiften/Base/API.h>
#include <Swiften/Base/Error.h>

namespace Swift {
    class SWIFTEN_API TLSError : public Error {
        public:
            typedef std::shared_ptr<TLSError> ref;

            enum Type {
                UnknownError,
                CertificateCardRemoved
            };

            TLSError(Type type = UnknownError) : type(type) {}

            Type getType() const {
                return type;
            }

        private:
            Type type;
    };
}
