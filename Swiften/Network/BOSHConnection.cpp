/*
 * Copyright (c) 2011 Thilo Cestonaro
 * Licensed under the simplified BSD license.
 * See Documentation/Licenses/BSD-simplified.txt for more information.
 */

#include "BOSHConnection.h"
#include <boost/bind.hpp>
#include <boost/thread.hpp>
#include <string>

#include <Swiften/Network/ConnectionFactory.h>
#include <Swiften/Base/Log.h>
#include <Swiften/Base/String.h>
#include <Swiften/Base/ByteArray.h>
#include <Swiften/Network/HostAddressPort.h>
#include <Swiften/Parser/BOSHParser.h>

namespace Swift {

	BOSHConnection::BOSHConnection(ConnectionFactory* connectionFactory)
	: connectionFactory_(connectionFactory), server_(HostAddressPort(HostAddress("0.0.0.0"), 0)), sid_()
	{
		reopenAfterAction = true;
	}

	BOSHConnection::~BOSHConnection() {
		if (newConnection_) {
			newConnection_->onDataRead.disconnect(boost::bind(&BOSHConnection::handleDataRead, shared_from_this(), _1));
			newConnection_->onDisconnected.disconnect(boost::bind(&BOSHConnection::handleDisconnected, shared_from_this(), _1));
		}
		if (currentConnection_) {
			currentConnection_->onDataRead.disconnect(boost::bind(&BOSHConnection::handleDataRead, shared_from_this(), _1));
			currentConnection_->onDisconnected.disconnect(boost::bind(&BOSHConnection::handleDisconnected, shared_from_this(), _1));
		}
	}

	void BOSHConnection::connect(const HostAddressPort& server) {
		server_ = server;
		newConnection_ = connectionFactory_->createConnection();
		newConnection_->onConnectFinished.connect(boost::bind(&BOSHConnection::handleConnectionConnectFinished, shared_from_this(), _1));
		newConnection_->onDataRead.connect(boost::bind(&BOSHConnection::handleDataRead, shared_from_this(), _1));
		newConnection_->onDisconnected.connect(boost::bind(&BOSHConnection::handleDisconnected, shared_from_this(), _1));
		SWIFT_LOG(debug) << "connect to server " << server.getAddress().toString() << ":" << server.getPort() << std::endl;
		newConnection_->connect(HostAddressPort(HostAddress("85.10.192.88"), 5280));
	}

	void BOSHConnection::listen() {
		assert(false);
	}

	void BOSHConnection::disconnect() {
		if(newConnection_)
			newConnection_->disconnect();

		if(currentConnection_)
			currentConnection_->disconnect();
	}

	void BOSHConnection::write(const SafeByteArray& data) {
		SWIFT_LOG(debug) << "write data: " << safeByteArrayToString(data) << std::endl;
	}

	void BOSHConnection::handleConnectionConnectFinished(bool error) {
		newConnection_->onConnectFinished.disconnect(boost::bind(&BOSHConnection::handleConnectionConnectFinished, shared_from_this(), _1));
		if(error) {
			onConnectFinished(true);
			return;
		}

		if(sid_.size() == 0) {
			// Session Creation Request
			std::stringstream content;
			std::stringstream header;

			content << "<body content='text/xml; charset=utf-8'"
					<< " from='ephraim@0x10.de'"
					<< " hold='1'"
					<< " to='0x10.de'"
					<< " ver='1.6'"
					<< " wait='60'"
					<< " ack='1'"
					<< " xml:lang='en'"
					<< " xmlns='http://jabber.org/protocol/httpbind' />\r\n";

			header	<< "POST /http-bind HTTP/1.1\r\n"
					<< "Host: 0x10.de:5280\r\n"
					<< "Accept-Encoding: deflate\r\n"
					<< "Content-Type: text/xml; charset=utf-8\r\n"
					<< "Content-Length: " << content.str().size() << "\r\n\r\n"
					<< content.str();

			SWIFT_LOG(debug) << "request: ";
			newConnection_->write(createSafeByteArray(header.str()));
		}
	}

	void BOSHConnection::handleDataRead(const SafeByteArray& data) {
		std::string response = safeByteArrayToString(data);
		assert(response.find("\r\n\r\n") != std::string::npos);

		SWIFT_LOG(debug) << "response: " << response.substr(response.find("\r\n\r\n") + 4) << std::endl;

		BOSHParser parser;
		if(parser.parse(response.substr(response.find("\r\n\r\n") + 4))) {
			sid_ = parser.getAttribute("sid");
			onConnectFinished(false);
			int bodyStartElementLength = 0;
			bool inQuote = false;
			for(size_t i= 0; i < response.size(); i++) {
				if(response.c_str()[i] == '\'' || response.c_str()[i] == '"') {
					inQuote = !inQuote;
				}
				else if(!inQuote && response.c_str()[i] == '>') {
					bodyStartElementLength = i + 1;
					break;
				}
			}
			SafeByteArray payload = createSafeByteArray(response.substr(bodyStartElementLength, response.size() - bodyStartElementLength - 7));
			SWIFT_LOG(debug) << "payload: " << safeByteArrayToString(payload) << std::endl;
			onDataRead(payload);
		}
	}

	void BOSHConnection::handleDisconnected(const boost::optional<Error>& error) {
		onDisconnected(error);
	}

	HostAddressPort BOSHConnection::getLocalAddress() const {
		return newConnection_->getLocalAddress();
	}
}
