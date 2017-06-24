/*
 * Copyright (c) 2010-2017 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <map>
#include <memory>
#include <string>

#include <Swiften/Base/API.h>
#include <Swiften/FileTransfer/ReadBytestream.h>
#include <Swiften/FileTransfer/SOCKS5BytestreamRegistry.h>
#include <Swiften/JID/JID.h>
#include <Swiften/Network/ConnectionServer.h>

namespace Swift {
    class SOCKS5BytestreamServerSession;

    class SWIFTEN_API SOCKS5BytestreamServer {
        public:
            SOCKS5BytestreamServer(
                    std::shared_ptr<ConnectionServer> connectionServer,
                    SOCKS5BytestreamRegistry* registry);

            HostAddressPort getAddressPort() const;

            void start();
            void stop();

            std::vector< std::shared_ptr<SOCKS5BytestreamServerSession> > getSessions(const std::string& id) const;

        private:
            void handleNewConnection(std::shared_ptr<Connection> connection);
            void handleSessionFinished(std::shared_ptr<SOCKS5BytestreamServerSession>);

        private:
            friend class SOCKS5BytestreamServerSession;

            std::shared_ptr<ConnectionServer> connectionServer;
            SOCKS5BytestreamRegistry* registry;
            std::vector<std::shared_ptr<SOCKS5BytestreamServerSession> > sessions;
    };
}

