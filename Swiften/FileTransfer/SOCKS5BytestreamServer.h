/*
 * Copyright (c) 2010-2016 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <map>
#include <string>

#include <boost/shared_ptr.hpp>

#include <Swiften/Base/API.h>
#include <Swiften/FileTransfer/ReadBytestream.h>
#include <Swiften/FileTransfer/SOCKS5BytestreamRegistry.h>
#include <Swiften/JID/JID.h>
#include <Swiften/Network/ConnectionServer.h>

namespace Swift {
    class SOCKS5BytestreamServerSession;
    class CryptoProvider;

    class SWIFTEN_API SOCKS5BytestreamServer {
        public:
            SOCKS5BytestreamServer(
                    boost::shared_ptr<ConnectionServer> connectionServer,
                    SOCKS5BytestreamRegistry* registry);

            HostAddressPort getAddressPort() const;

            void start();
            void stop();

            std::vector< boost::shared_ptr<SOCKS5BytestreamServerSession> > getSessions(const std::string& id) const;

        private:
            void handleNewConnection(boost::shared_ptr<Connection> connection);
            void handleSessionFinished(boost::shared_ptr<SOCKS5BytestreamServerSession>);

        private:
            friend class SOCKS5BytestreamServerSession;

            boost::shared_ptr<ConnectionServer> connectionServer;
            SOCKS5BytestreamRegistry* registry;
            std::vector<boost::shared_ptr<SOCKS5BytestreamServerSession> > sessions;
    };
}

