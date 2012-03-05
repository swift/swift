/*
 * Copyright (c) 2011 Kevin Smith
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */
#include <Swiften/Network/BOSHConnectionPool.h>

#include <climits>

#include <boost/bind.hpp>
#include <boost/lexical_cast.hpp>

#include <Swiften/Base/foreach.h>
#include <Swiften/Base/SafeString.h>
#include <Swiften/Network/TLSConnectionFactory.h>
#include <Swiften/Network/HTTPConnectProxiedConnectionFactory.h>
#include <Swiften/Network/CachingNameOnlyDomainNameResolver.h>

namespace Swift {
BOSHConnectionPool::BOSHConnectionPool(const URL& boshURL, DomainNameResolver* realResolver, ConnectionFactory* connectionFactoryParameter, XMLParserFactory* parserFactory, TLSContextFactory* tlsFactory, TimerFactory* timerFactory, EventLoop* eventLoop, const std::string& to, unsigned long long initialRID, const URL& boshHTTPConnectProxyURL, const SafeString& boshHTTPConnectProxyAuthID, const SafeString& boshHTTPConnectProxyAuthPassword) :
		boshURL(boshURL),
		connectionFactory(connectionFactoryParameter),
		xmlParserFactory(parserFactory),
		tlsFactory(tlsFactory),
		timerFactory(timerFactory),
		rid(initialRID),
		pendingTerminate(false),
		to(to),
		requestLimit(2),
		restartCount(0),
		pendingRestart(false) {

	if (!boshHTTPConnectProxyURL.empty()) {
		if (boshHTTPConnectProxyURL.getScheme() == "https") {
			connectionFactory = new TLSConnectionFactory(tlsFactory, connectionFactory);
			myConnectionFactories.push_back(connectionFactory);
		}
		connectionFactory = new HTTPConnectProxiedConnectionFactory(realResolver, connectionFactory, timerFactory, eventLoop, boshHTTPConnectProxyURL.getHost(), boshHTTPConnectProxyURL.getPort(), boshHTTPConnectProxyAuthID, boshHTTPConnectProxyAuthPassword);
	}
	if (boshURL.getScheme() == "https") {
		connectionFactory = new TLSConnectionFactory(tlsFactory, connectionFactory);
		myConnectionFactories.push_back(connectionFactory);
	}
	resolver = new CachingNameOnlyDomainNameResolver(realResolver, eventLoop);
	createConnection();
}

BOSHConnectionPool::~BOSHConnectionPool() {
	close();
	foreach (ConnectionFactory* factory, myConnectionFactories) {
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

void BOSHConnectionPool::writeFooter() {
	pendingTerminate = true;
	tryToSendQueuedData();
}

void BOSHConnectionPool::close() {
	/* TODO: Send a terminate here. */
	std::vector<BOSHConnection::ref> connectionCopies = connections;
	foreach (BOSHConnection::ref connection, connectionCopies) {
		if (connection) {
			connection->disconnect();
			destroyConnection(connection);
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
		onSessionTerminated(boost::make_shared<BOSHError>(BOSHError::UndefinedCondition));
		/*TODO: We can probably manage to not terminate the stream here and use the rid/ack retry
		 * logic to just swallow the error and try again (some number of tries).
		 */
	}
	else {
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
	foreach (BOSHConnection::ref connection, connections) {
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
			foreach (const SafeByteArray& datum, dataQueue) {
				data.insert(data.end(), datum.begin(), datum.end());
			}
			suitableConnection->write(data);
			dataQueue.clear();
		}
		else if (pendingTerminate) {
			rid++;
			suitableConnection->setRID(rid);
			suitableConnection->terminateStream();
			onSessionTerminated(boost::shared_ptr<BOSHError>());
		}
	}
	if (!pendingTerminate) {
		/* Ensure there's always a session waiting to read data for us */
		bool pending = false;
		foreach (BOSHConnection::ref connection, connections) {
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
	handleSessionTerminated(boost::make_shared<BOSHError>(BOSHError::UndefinedCondition));
}

void BOSHConnectionPool::handleConnectionDisconnected(bool error, BOSHConnection::ref connection) {
	destroyConnection(connection);
	if (false && error) {
		handleSessionTerminated(boost::make_shared<BOSHError>(BOSHError::UndefinedCondition));
	}
	else {
		/* We might have just freed up a connection slot to send with */
		tryToSendQueuedData();
	}
}

boost::shared_ptr<BOSHConnection> BOSHConnectionPool::createConnection() {
	Connector::ref connector = Connector::create(boshURL.getHost(), resolver, connectionFactory, timerFactory, boshURL.getPort());
	BOSHConnection::ref connection = BOSHConnection::create(boshURL, connector, xmlParserFactory);
	connection->onXMPPDataRead.connect(boost::bind(&BOSHConnectionPool::handleDataRead, this, _1));
	connection->onSessionStarted.connect(boost::bind(&BOSHConnectionPool::handleSessionStarted, this, _1, _2));
	connection->onBOSHDataRead.connect(boost::bind(&BOSHConnectionPool::handleBOSHDataRead, this, _1));
	connection->onBOSHDataWritten.connect(boost::bind(&BOSHConnectionPool::handleBOSHDataWritten, this, _1));
	connection->onDisconnected.connect(boost::bind(&BOSHConnectionPool::handleConnectionDisconnected, this, _1, connection));
	connection->onConnectFinished.connect(boost::bind(&BOSHConnectionPool::handleConnectFinished, this, _1, connection));
	connection->onSessionTerminated.connect(boost::bind(&BOSHConnectionPool::handleSessionTerminated, this, _1));
	connection->onHTTPError.connect(boost::bind(&BOSHConnectionPool::handleHTTPError, this, _1));
	connection->connect();
	connections.push_back(connection);
	return connection;
}

void BOSHConnectionPool::destroyConnection(boost::shared_ptr<BOSHConnection> connection) {
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
