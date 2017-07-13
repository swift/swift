/*
 * Copyright (c) 2015-2017 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <boost/signals2.hpp>

#include <Swiften/Base/API.h>
#include <Swiften/FileTransfer/IBBSendSession.h>
#include <Swiften/FileTransfer/TransportSession.h>

namespace Swift {

class SWIFTEN_API IBBSendTransportSession : public TransportSession {
    public:
        IBBSendTransportSession(std::shared_ptr<IBBSendSession> session);
        virtual ~IBBSendTransportSession() override;

        virtual void start() override;
        virtual void stop() override;

    private:
        std::shared_ptr<IBBSendSession> session;
        boost::signals2::scoped_connection finishedConnection;
        boost::signals2::scoped_connection bytesSentConnection;
};

}
