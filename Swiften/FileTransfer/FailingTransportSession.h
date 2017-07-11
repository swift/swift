/*
 * Copyright (c) 2015-2017 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <Swiften/Base/API.h>
#include <Swiften/FileTransfer/TransportSession.h>

namespace Swift {

class SWIFTEN_API FailingTransportSession : public TransportSession {
    public:
        virtual ~FailingTransportSession() SWIFTEN_OVERRIDE;

        virtual void start() SWIFTEN_OVERRIDE;
        virtual void stop() SWIFTEN_OVERRIDE;
};

}
