/*
 * Copyright (c) 2010-2016 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <boost/asio/io_service.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <boost/enable_shared_from_this.hpp>
#include <boost/thread/mutex.hpp>

#include <Swiften/Base/API.h>
#include <Swiften/Base/SafeByteArray.h>
#include <Swiften/EventLoop/EventOwner.h>
#include <Swiften/Network/Connection.h>
#include <Swiften/TLS/Certificate.h>
#include <Swiften/TLS/CertificateVerificationError.h>
#include <Swiften/TLS/CertificateWithKey.h>

namespace boost {
    class thread;
    namespace system {
        class error_code;
    }
}

namespace Swift {
    class EventLoop;

    class SWIFTEN_API BoostConnection : public Connection, public EventOwner, public boost::enable_shared_from_this<BoostConnection> {
        public:
            typedef boost::shared_ptr<BoostConnection> ref;

            virtual ~BoostConnection();

            static ref create(boost::shared_ptr<boost::asio::io_service> ioService, EventLoop* eventLoop) {
                return ref(new BoostConnection(ioService, eventLoop));
            }

            virtual void listen();
            virtual void connect(const HostAddressPort& address);
            virtual void disconnect();
            virtual void write(const SafeByteArray& data);

            boost::asio::ip::tcp::socket& getSocket() {
                return socket_;
            }

            virtual HostAddressPort getLocalAddress() const;
            virtual HostAddressPort getRemoteAddress() const;

            bool setClientCertificate(CertificateWithKey::ref cert);

            Certificate::ref getPeerCertificate() const;
            std::vector<Certificate::ref> getPeerCertificateChain() const;
            boost::shared_ptr<CertificateVerificationError> getPeerCertificateVerificationError() const;

        private:
            BoostConnection(boost::shared_ptr<boost::asio::io_service> ioService, EventLoop* eventLoop);

            void handleConnectFinished(const boost::system::error_code& error);
            void handleSocketRead(const boost::system::error_code& error, size_t bytesTransferred);
            void handleDataWritten(const boost::system::error_code& error);
            void doRead();
            void doWrite(const SafeByteArray& data);
            void closeSocket();

        private:
            EventLoop* eventLoop;
            boost::shared_ptr<boost::asio::io_service> ioService;
            boost::asio::ip::tcp::socket socket_;
            boost::shared_ptr<SafeByteArray> readBuffer_;
            boost::mutex writeMutex_;
            bool writing_;
            SafeByteArray writeQueue_;
            bool closeSocketAfterNextWrite_;
            boost::mutex readCloseMutex_;
    };
}
