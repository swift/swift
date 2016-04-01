/*
 * Copyright (c) 2015-2016 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <Swiften/Base/API.h>
#include <Swiften/Base/boost_bsignals.h>
#include <Swiften/FileTransfer/IBBReceiveSession.h>
#include <Swiften/FileTransfer/TransportSession.h>

namespace Swift {

class SWIFTEN_API IBBReceiveTransportSession : public TransportSession {
    public:
        IBBReceiveTransportSession(std::shared_ptr<IBBReceiveSession> session);
        virtual ~IBBReceiveTransportSession();

        virtual void start() SWIFTEN_OVERRIDE;
        virtual void stop() SWIFTEN_OVERRIDE;

    private:
        std::shared_ptr<IBBReceiveSession> session;
        boost::bsignals::scoped_connection finishedConnection;
        boost::bsignals::scoped_connection bytesSentConnection;
};

}
