/*
 * Copyright (c) 2011-2016 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */
#include <Swiften/Network/BOSHConnectionPool.h>

#include <climits>

#include <boost/bind.hpp>
#include <boost/lexical_cast.hpp>

#include <Swiften/Base/Log.h>
#include <Swiften/Base/SafeString.h>
#include <Swiften/Network/CachingDomainNameResolver.h>
#include <Swiften/Network/HTTPConnectProxiedConnectionFactory.h>

namespace Swift {
BOSHConnectionPool::BOSHConnectionPool(const URL& boshURL, DomainNameResolver* realResolver, ConnectionFactory* connectionFactoryParameter, XMLParserFactory* parserFactory, TLSContextFactory* tlsFactory, TimerFactory* timerFactory, EventLoop* eventLoop, const std::string& to, unsigned long long initialRID, const URL& boshHTTPConnectProxyURL, const SafeString& boshHTTPConnectProxyAuthID, const SafeString& boshHTTPConnectProxyAuthPassword, const TLSOptions& tlsOptions, std::shared_ptr<HTTPTrafficFilter> trafficFilter) :
        boshURL(boshURL),
        connectionFactory(connectionFactoryParameter),
        xmlParserFactory(parserFactory),
        timerFactory(timerFactory),
        rid(initialRID),
        pendingTerminate(false),
        to(to),
        requestLimit(2),
        restartCount(0),
        pendingRestart(false),
        tlsContextFactory_(tlsFactory),
        tlsOptions_(tlsOptions) {

    if (!boshHTTPConnectProxyURL.isEmpty()) {
        connectionFactory = new HTTPConnectProxiedConnectionFactory(realResolver, connectionFactory, timerFactory, boshHTTPConnectProxyURL.getHost(), URL::getPortOrDefaultPort(boshHTTPConnectProxyURL), boshHTTPConnectProxyAuthID, boshHTTPConnectProxyAuthPassword, trafficFilter);
    }
    resolver = new CachingDomainNameResolver(realResolver, eventLoop);
}

BOSHConnectionPool::~BOSHConnectionPool() {
    /* Don't do a normal close here. Instead kill things forcibly, as close() or writeFooter() will already have been called */
    std::vector<BOSHConnection::ref> connectionCopies = connections;
    for (auto&& connection : connectionCopies) {
        if (connection) {
            destroyConnection(connection);
            connection->disconnect();
        }
    }
    for (auto factory : myConnectionFactories) {
        delete factory;
    }
    delete resolver;
}

void BOSHConnectionPool::write(const SafeByteArray& data) {
    dataQueue.push_back(data);
    tryToSendQueuedData();
}

void BOSHConnectionPool::handleDataRead(const SafeByteArray& data) {
    onXMPPDataRead(data);
    tryToSendQueuedData(); /* Will rebalance the connections */
}

void BOSHConnectionPool::restartStream() {
    BOSHConnection::ref connection = getSuitableConnection();
    if (connection) {
        pendingRestart = false;
        rid++;
        connection->setRID(rid);
        connection->restartStream();
        restartCount++;
    }
    else {
        pendingRestart = true;
    }
}

void BOSHConnectionPool::setTLSCertificate(CertificateWithKey::ref certWithKey) {
    clientCertificate = certWithKey;
}

bool BOSHConnectionPool::isTLSEncrypted() const {
    return !pinnedCertificateChain_.empty();
}

Certificate::ref BOSHConnectionPool::getPeerCertificate() const {
    Certificate::ref peerCertificate;
    if (!pinnedCertificateChain_.empty()) {
        peerCertificate = pinnedCertificateChain_[0];
    }
    return peerCertificate;
}

std::vector<Certificate::ref> BOSHConnectionPool::getPeerCertificateChain() const {
    return pinnedCertificateChain_;
}

std::shared_ptr<CertificateVerificationError> BOSHConnectionPool::getPeerCertificateVerificationError() const {
    return lastVerificationError_;
}

void BOSHConnectionPool::writeFooter() {
    pendingTerminate = true;
    tryToSendQueuedData();
}

void BOSHConnectionPool::open() {
    createConnection();
}

void BOSHConnectionPool::close() {
    if (!sid.empty()) {
        writeFooter();
    }
    else {
        pendingTerminate = true;
        std::vector<BOSHConnection::ref> connectionCopies = connections;
        for (auto&& connection : connectionCopies) {
            if (connection) {
                connection->disconnect();
            }
        }
    }
}

void BOSHConnectionPool::handleSessionStarted(const std::string& sessionID, size_t requests) {
    sid = sessionID;
    requestLimit = requests;
    onSessionStarted();
}

void BOSHConnectionPool::handleConnectFinished(bool error, BOSHConnection::ref connection) {
    if (error) {
        onSessionTerminated(std::make_shared<BOSHError>(BOSHError::UndefinedCondition));
        /*TODO: We can probably manage to not terminate the stream here and use the rid/ack retry
         * logic to just swallow the error and try again (some number of tries).
         */
    }
    else {
        if (connection->getPeerCertificate() && pinnedCertificateChain_.empty()) {
            pinnedCertificateChain_ = connection->getPeerCertificateChain();
        }
        if (!pinnedCertificateChain_.empty()) {
            lastVerificationError_ = connection->getPeerCertificateVerificationError();
            onTLSConnectionEstablished();
        }

        if (sid.empty()) {
            connection->startStream(to, rid);
        }
        if (pendingRestart) {
            restartStream();
        }
        tryToSendQueuedData();
    }
}

BOSHConnection::ref BOSHConnectionPool::getSuitableConnection() {
    BOSHConnection::ref suitableConnection;
    for (auto&& connection : connections) {
        if (connection->isReadyToSend()) {
            suitableConnection = connection;
            break;
        }
    }

    if (!suitableConnection && connections.size() < requestLimit) {
        /* This is not a suitable connection because it won't have yet connected and added TLS if needed. */
        BOSHConnection::ref newConnection = createConnection();
        newConnection->setSID(sid);
    }
    assert(connections.size() <= requestLimit);
    assert((!suitableConnection) || suitableConnection->isReadyToSend());
    return suitableConnection;
}

void BOSHConnectionPool::tryToSendQueuedData() {
    if (sid.empty()) {
        /* If we've not got as far as stream start yet, pend */
        return;
    }

    BOSHConnection::ref suitableConnection = getSuitableConnection();
    bool toSend = !dataQueue.empty();
    if (suitableConnection) {
        if (toSend) {
            rid++;
            suitableConnection->setRID(rid);
            SafeByteArray data;
            for (const auto& datum : dataQueue) {
                data.insert(data.end(), datum.begin(), datum.end());
            }
            suitableConnection->write(data);
            dataQueue.clear();
        }
        else if (pendingTerminate) {
            rid++;
            suitableConnection->setRID(rid);
            suitableConnection->terminateStream();
            sid = "";
            close();
        }
    }
    if (!pendingTerminate) {
        /* Ensure there's always a session waiting to read data for us */
        bool pending = false;
        for (auto&& connection : connections) {
            if (connection && !connection->isReadyToSend()) {
                pending = true;
            }
        }
        if (!pending) {
            if (restartCount >= 1) {
                /* Don't open a second connection until we've restarted the stream twice - i.e. we've authed and resource bound.*/
                if (suitableConnection) {
                    rid++;
                    suitableConnection->setRID(rid);
                    suitableConnection->write(createSafeByteArray(""));
                }
                else {
                /* My thought process I went through when writing this, to aid anyone else confused why this can happen...
                 *
                 * What to do here? I think this isn't possible.
                   If you didn't have two connections, suitable would have made one.
                   If you have two connections and neither is suitable, pending would be true.
                   If you have a non-pending connection, it's suitable.

                   If I decide to do something here, remove assert above.

                   Ah! Yes, because there's a period between creating the connection and it being connected. */
                }
            }
        }
    }
}

void BOSHConnectionPool::handleHTTPError(const std::string& /*errorCode*/) {
    handleSessionTerminated(std::make_shared<BOSHError>(BOSHError::UndefinedCondition));
}

void BOSHConnectionPool::handleConnectionDisconnected(bool/* error*/, BOSHConnection::ref connection) {
    destroyConnection(connection);
    if (pendingTerminate && sid.empty() && connections.empty()) {
        handleSessionTerminated(BOSHError::ref());
    }
    //else if (error) {
    //    handleSessionTerminated(std::make_shared<BOSHError>(BOSHError::UndefinedCondition));
    //}
    else {
        /* We might have just freed up a connection slot to send with */
        tryToSendQueuedData();
    }
}

std::shared_ptr<BOSHConnection> BOSHConnectionPool::createConnection() {
    Connector::ref connector = Connector::create(boshURL.getHost(), URL::getPortOrDefaultPort(boshURL), boost::optional<std::string>(), resolver, connectionFactory, timerFactory);
    BOSHConnection::ref connection = BOSHConnection::create(boshURL, connector, xmlParserFactory, tlsContextFactory_, tlsOptions_);
    connection->onXMPPDataRead.connect(boost::bind(&BOSHConnectionPool::handleDataRead, this, _1));
    connection->onSessionStarted.connect(boost::bind(&BOSHConnectionPool::handleSessionStarted, this, _1, _2));
    connection->onBOSHDataRead.connect(boost::bind(&BOSHConnectionPool::handleBOSHDataRead, this, _1));
    connection->onBOSHDataWritten.connect(boost::bind(&BOSHConnectionPool::handleBOSHDataWritten, this, _1));
    connection->onDisconnected.connect(boost::bind(&BOSHConnectionPool::handleConnectionDisconnected, this, _1, connection));
    connection->onConnectFinished.connect(boost::bind(&BOSHConnectionPool::handleConnectFinished, this, _1, connection));
    connection->onSessionTerminated.connect(boost::bind(&BOSHConnectionPool::handleSessionTerminated, this, _1));
    connection->onHTTPError.connect(boost::bind(&BOSHConnectionPool::handleHTTPError, this, _1));

    if (boshURL.getScheme() == "https") {
        bool success = connection->setClientCertificate(clientCertificate);
        SWIFT_LOG(debug) << "setClientCertificate, success: " << success << std::endl;
    }

    connection->connect();
    connections.push_back(connection);
    return connection;
}

void BOSHConnectionPool::destroyConnection(std::shared_ptr<BOSHConnection> connection) {
    connections.erase(std::remove(connections.begin(), connections.end(), connection), connections.end());
    connection->onXMPPDataRead.disconnect(boost::bind(&BOSHConnectionPool::handleDataRead, this, _1));
    connection->onSessionStarted.disconnect(boost::bind(&BOSHConnectionPool::handleSessionStarted, this, _1, _2));
    connection->onBOSHDataRead.disconnect(boost::bind(&BOSHConnectionPool::handleBOSHDataRead, this, _1));
    connection->onBOSHDataWritten.disconnect(boost::bind(&BOSHConnectionPool::handleBOSHDataWritten, this, _1));
    connection->onDisconnected.disconnect(boost::bind(&BOSHConnectionPool::handleConnectionDisconnected, this, _1, connection));
    connection->onConnectFinished.disconnect(boost::bind(&BOSHConnectionPool::handleConnectFinished, this, _1, connection));
    connection->onSessionTerminated.disconnect(boost::bind(&BOSHConnectionPool::handleSessionTerminated, this, _1));
    connection->onHTTPError.disconnect(boost::bind(&BOSHConnectionPool::handleHTTPError, this, _1));
}

void BOSHConnectionPool::handleSessionTerminated(BOSHError::ref error) {
    onSessionTerminated(error);
}

void BOSHConnectionPool::handleBOSHDataRead(const SafeByteArray& data) {
    onBOSHDataRead(data);
}

void BOSHConnectionPool::handleBOSHDataWritten(const SafeByteArray& data) {
    onBOSHDataWritten(data);
}

}
