/*
 * Copyright (c) 2015-2017 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <boost/bind.hpp>
#include <boost/signals2.hpp>

#include <Swiften/Base/API.h>
#include <Swiften/FileTransfer/ReadBytestream.h>
#include <Swiften/FileTransfer/TransportSession.h>
#include <Swiften/FileTransfer/WriteBytestream.h>

namespace Swift {

template <typename T>
class SWIFTEN_API S5BTransportSession : public TransportSession {
    public:
        S5BTransportSession(
                std::shared_ptr<T> session,
                std::shared_ptr<ReadBytestream> readStream) :
                    session(session),
                    readStream(readStream) {
            initialize();
        }

        S5BTransportSession(
                std::shared_ptr<T> session,
                std::shared_ptr<WriteBytestream> writeStream) :
                    session(session),
                    writeStream(writeStream) {
            initialize();
        }

        virtual ~S5BTransportSession() override {
        }

        virtual void start() override {
            if (readStream) {
                session->startSending(readStream);
            }
            else {
                session->startReceiving(writeStream);
            }
        }

        virtual void stop() override {
            session->stop();
        }

    private:
        void initialize() {
            finishedConnection = session->onFinished.connect(boost::bind(boost::ref(onFinished), _1));
            bytesSentConnection = session->onBytesSent.connect(boost::bind(boost::ref(onBytesSent), _1));
        }

    private:
        std::shared_ptr<T> session;
        std::shared_ptr<ReadBytestream> readStream;
        std::shared_ptr<WriteBytestream> writeStream;

        boost::signals2::scoped_connection finishedConnection;
        boost::signals2::scoped_connection bytesSentConnection;
};

}
