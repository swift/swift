/*
 * Copyright (c) 2015 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <Swiften/Base/API.h>
#include <Swiften/FileTransfer/TransportSession.h>

namespace Swift {

class SWIFTEN_API FailingTransportSession : public TransportSession {
    public:
        virtual ~FailingTransportSession() {
        }

        virtual void start() SWIFTEN_OVERRIDE {
            assert(false);
            onFinished(FileTransferError(FileTransferError::PeerError));
        }

        virtual void stop() SWIFTEN_OVERRIDE {
        }
};

}
