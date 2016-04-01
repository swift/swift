/*
 * Copyright (c) 2010-2016 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <memory>

#include <Swiften/Base/API.h>
#include <Swiften/Base/Error.h>

namespace Swift {
    class SWIFTEN_API CertificateVerificationError : public Error {
        public:
            typedef std::shared_ptr<CertificateVerificationError> ref;

            enum Type {
                UnknownError,
                Expired,
                NotYetValid,
                SelfSigned,
                Rejected,
                Untrusted,
                InvalidPurpose,
                PathLengthExceeded,
                InvalidSignature,
                InvalidCA,
                InvalidServerIdentity,
                Revoked,
                RevocationCheckFailed
            };

            CertificateVerificationError(Type type = UnknownError) : type(type) {}

            Type getType() const {
                return type;
            }

        private:
            Type type;
    };
}
