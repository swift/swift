/*
 * Copyright (c) 2011 Tobias Markmann
 * Licensed under the simplified BSD license.
 * See Documentation/Licenses/BSD-simplified.txt for more information.
 */

/*
 * Copyright (c) 2015-2016 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */


#pragma once

#include <map>
#include <string>
#include <utility>
#include <vector>

#include <Swiften/Base/API.h>
#include <Swiften/Elements/S5BProxyRequest.h>
#include <Swiften/FileTransfer/SOCKS5BytestreamClientSession.h>
#include <Swiften/FileTransfer/SOCKS5BytestreamProxyFinder.h>
#include <Swiften/JID/JID.h>

namespace Swift {
    class TimerFactory;
    class ConnectionFactory;
    class DomainNameResolver;
    class DomainNameResolveError;
    class IQRouter;

    /**
     *    - manages list of working S5B proxies
     *    - creates initial connections (for the candidates you provide)
     */
    class SWIFTEN_API SOCKS5BytestreamProxiesManager {
        public:
            SOCKS5BytestreamProxiesManager(ConnectionFactory*, TimerFactory*, DomainNameResolver*, IQRouter*, const JID&);
            ~SOCKS5BytestreamProxiesManager();

            void addS5BProxy(S5BProxyRequest::ref);

            /*
             * Returns a list of external S5B proxies. If the optinal return value is not initialized a discovery process has been started and
             * onDiscoveredProxiesChanged signal will be emitted when it is finished.
             */
            const boost::optional<std::vector<S5BProxyRequest::ref> >& getOrDiscoverS5BProxies();

            void connectToProxies(const std::string& sessionID);
            std::shared_ptr<SOCKS5BytestreamClientSession> getProxySessionAndCloseOthers(const JID& proxyJID, const std::string& sessionID);

            std::shared_ptr<SOCKS5BytestreamClientSession> createSOCKS5BytestreamClientSession(HostAddressPort addressPort, const std::string& destAddr);

        public:
            boost::signals2::signal<void ()> onDiscoveredProxiesChanged;

        private:
            void handleProxiesFound(std::vector<S5BProxyRequest::ref> proxyHosts);
            void handleNameLookupResult(const std::vector<HostAddress>&, boost::optional<DomainNameResolveError>, S5BProxyRequest::ref proxy);

            void queryForProxies();

            void handleProxySessionReady(const std::string& sessionID, const JID& jid, std::shared_ptr<SOCKS5BytestreamClientSession> session, bool error);
            void handleProxySessionFinished(const std::string& sessionID, const JID& jid, std::shared_ptr<SOCKS5BytestreamClientSession> session, boost::optional<FileTransferError> error);

        private:
            ConnectionFactory* connectionFactory_;
            TimerFactory* timerFactory_;
            DomainNameResolver* resolver_;
            IQRouter* iqRouter_;
            JID serviceRoot_;

            typedef std::vector<std::pair<JID, std::shared_ptr<SOCKS5BytestreamClientSession> > > ProxyJIDClientSessionVector;
            typedef std::map<std::string, ProxyJIDClientSessionVector> ProxySessionsMap;
            ProxySessionsMap proxySessions_;

            std::shared_ptr<SOCKS5BytestreamProxyFinder> proxyFinder_;

            boost::optional<std::vector<S5BProxyRequest::ref> > localS5BProxies_;
    };

}
