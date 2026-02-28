/*
 * Copyright (c) 2012-2019 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <memory>
#include <string>

#include <Swiften/Base/API.h>
#include <Swiften/Base/Error.h>

namespace Swift {
    class SWIFTEN_API TLSError : public Error {
        public:
            typedef std::shared_ptr<TLSError> ref;

            enum Type {
                UnknownError,
                CertificateCardRemoved,
                AcceptFailed,
                ConnectFailed
            };

            TLSError(Type type = UnknownError, std::string message = "") : type_(type), message_(std::move(message)) {}

            Type getType() const {
                return type_;
            }

            const std::string& getMessage() const {
                return message_;
            }

        private:
            Type type_;
            std::string message_;
    };
}
