/*
 * Copyright (c) 2010-2016 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <memory>

#include <boost/signals2.hpp>

#include <Swiften/Base/API.h>
#include <Swiften/FileTransfer/FileTransferError.h>
#include <Swiften/FileTransfer/ReadBytestream.h>
#include <Swiften/FileTransfer/WriteBytestream.h>
#include <Swiften/Network/Connection.h>

namespace Swift {
    class SOCKS5BytestreamRegistry;

    class SWIFTEN_API SOCKS5BytestreamServerSession {
        public:
        typedef std::shared_ptr<SOCKS5BytestreamServerSession> ref;

        public:
            enum State {
                Initial,
                WaitingForAuthentication,
                WaitingForRequest,
                ReadyForTransfer,
                ReadingData,
                WritingData,
                Finished
            };

            SOCKS5BytestreamServerSession(std::shared_ptr<Connection> connection, SOCKS5BytestreamRegistry* registry);
            ~SOCKS5BytestreamServerSession();

            void setChunkSize(int chunkSize) {
                this->chunkSize = chunkSize;
            }

            void start();
            void stop();

            void startSending(std::shared_ptr<ReadBytestream>);
            void startReceiving(std::shared_ptr<WriteBytestream>);

            HostAddressPort getAddressPort() const;

            boost::signals2::signal<void (boost::optional<FileTransferError>)> onFinished;
            boost::signals2::signal<void (unsigned long long)> onBytesSent;

            const std::string& getStreamID() const {
                return streamID;
            }

        private:
            void finish(const boost::optional<FileTransferError>& error = boost::optional<FileTransferError>());
            void process();
            void handleDataRead(std::shared_ptr<SafeByteArray>);
            void handleDisconnected(const boost::optional<Connection::Error>&);
            void handleDataAvailable();
            void sendData();

        private:
            std::shared_ptr<Connection> connection;
            SOCKS5BytestreamRegistry* bytestreams;
            ByteArray unprocessedData;
            State state;
            int chunkSize;
            std::string streamID;
            std::shared_ptr<ReadBytestream> readBytestream;
            std::shared_ptr<WriteBytestream> writeBytestream;
            bool waitingForData;

            boost::signals2::connection disconnectedConnection;
            boost::signals2::connection dataReadConnection;
            boost::signals2::connection dataWrittenConnection;
            boost::signals2::connection dataAvailableConnection;

    };
}
