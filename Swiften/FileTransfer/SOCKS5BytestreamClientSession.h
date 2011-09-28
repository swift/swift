/*
 * Copyright (c) 2011 Tobias Markmann
 * Licensed under the simplified BSD license.
 * See Documentation/Licenses/BSD-simplified.txt for more information.
 */

#pragma once

#include <boost/shared_ptr.hpp>
#include <boost/optional.hpp>

#include <Swiften/Base/ByteArray.h>
#include <Swiften/FileTransfer/FileTransferError.h>
#include <Swiften/FileTransfer/WriteBytestream.h>
#include <Swiften/FileTransfer/ReadBytestream.h>
#include <Swiften/JID/JID.h>
#include <Swiften/Network/Connection.h>
#include <Swiften/Network/HostAddressPort.h>
#include <Swiften/Network/Timer.h>

namespace Swift {

class SOCKS5BytestreamRegistry;
class Connection;
class TimerFactory;

/**
 * A session which has been connected to a SOCKS5 server (requester).
 *
 */
class SOCKS5BytestreamClientSession {
public:
	enum State {
		Initial,
		Hello,
		Authenticating,
		Ready,
		Writing,
		Reading,
		Finished,
	};

public:
	typedef boost::shared_ptr<SOCKS5BytestreamClientSession> ref;

public:
	SOCKS5BytestreamClientSession(boost::shared_ptr<Connection> connection, const HostAddressPort&, const std::string&, TimerFactory*);

	void start();
	void stop();

	void startReceiving(boost::shared_ptr<WriteBytestream>);
	void startSending(boost::shared_ptr<ReadBytestream>);

	HostAddressPort getAddressPort() const;

	boost::signal<void (bool /*error*/)> onSessionReady;

	boost::signal<void (boost::optional<FileTransferError>)> onFinished;
	boost::signal<void (int)> onBytesSent;
	boost::signal<void (int)> onBytesReceived;

private:
	void process();
	void hello();
	void authenticate();

	void handleConnectFinished(bool error);
	void handleDataRead(boost::shared_ptr<SafeByteArray>);
	void handleDisconnected(const boost::optional<Connection::Error>&);
	void handleWeFailedTimeout();

	void finish(bool error);
	void sendData();

private:
	boost::shared_ptr<Connection> connection;
	HostAddressPort addressPort;
	std::string destination; // hexify(SHA1(sessionID + requester + target))

	State state;
	int destinationPort;

	ByteArray unprocessedData;
	ByteArray authenticateAddress;

	int chunkSize;
	boost::shared_ptr<WriteBytestream> writeBytestream;
	boost::shared_ptr<ReadBytestream> readBytestream;

	Timer::ref weFailedTimeout;
};

}
