/*
 * Copyright (c) 2010-2017 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <memory>
#include <vector>

#include <boost/signals2.hpp>

#include <Swiften/Base/API.h>
#include <Swiften/LinkLocal/LinkLocalService.h>
#include <Swiften/Network/Connection.h>

namespace Swift {
    class DNSSDQuerier;
    class DNSSDResolveHostnameQuery;
    class HostAddress;
    class ToplevelElement;

    class SWIFTEN_API LinkLocalConnector : public std::enable_shared_from_this<LinkLocalConnector> {
        public:
            LinkLocalConnector(
                    const LinkLocalService& service,
                    std::shared_ptr<DNSSDQuerier> querier,
                    std::shared_ptr<Connection> connection);
            ~LinkLocalConnector();

            const LinkLocalService& getService() const {
                return service;
            }

            void connect();
            void cancel();
            void queueElement(std::shared_ptr<ToplevelElement> element);

            const std::vector<std::shared_ptr<ToplevelElement> >& getQueuedElements() const {
                return queuedElements;
            }

            std::shared_ptr<Connection> getConnection() const {
                return connection;
            }

            boost::signals2::signal<void (bool)> onConnectFinished;

        private:
            void handleHostnameResolved(const boost::optional<HostAddress>& address);
            void handleConnected(bool error);

        private:
            LinkLocalService service;
            std::shared_ptr<DNSSDQuerier> querier;
            std::shared_ptr<DNSSDResolveHostnameQuery> resolveQuery;
            boost::signals2::connection resolveQueryHostNameResolvedConnection;
            std::shared_ptr<Connection> connection;
            boost::signals2::connection connectionConnectFinishedConnection;
            std::vector<std::shared_ptr<ToplevelElement> > queuedElements;
    };
}
