/*
 * Copyright (c) 2010-2016 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <Swiften/Base/API.h>

namespace Swift {
    class SWIFTEN_API FileTransferError {
        public:
            enum Type {
                UnknownError,
                PeerError,
                ReadError,
                WriteError,
                ClosedError
            };

            FileTransferError(Type type = UnknownError) : type(type) {}

            Type getType() const {
                return type;
            }

        private:
            Type type;
    };
}
