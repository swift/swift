/*
 * Copyright (c) 2010-2011 Thilo Cestonaro
 * Licensed under the simplified BSD license.
 * See Documentation/Licenses/BSD-simplified.txt for more information.
 */

#include <Swiften/Network/SOCKS5ProxiedConnection.h>

#include <iostream>
#include <boost/bind.hpp>
#include <boost/thread.hpp>

#include <Swiften/Network/ConnectionFactory.h>
#include <Swiften/Base/Log.h>
#include <Swiften/Base/String.h>
#include <Swiften/Base/ByteArray.h>
#include <Swiften/Network/HostAddressPort.h>

using namespace Swift;

SOCKS5ProxiedConnection::SOCKS5ProxiedConnection(ConnectionFactory* connectionFactory, const HostAddressPort& proxy) : connectionFactory_(connectionFactory), proxy_(proxy), server_(HostAddressPort(HostAddress("0.0.0.0"), 0)) {
	connected_ = false;
}

SOCKS5ProxiedConnection::~SOCKS5ProxiedConnection() {
	if (connection_) {
		connection_->onDataRead.disconnect(boost::bind(&SOCKS5ProxiedConnection::handleDataRead, shared_from_this(), _1));
		connection_->onDisconnected.disconnect(boost::bind(&SOCKS5ProxiedConnection::handleDisconnected, shared_from_this(), _1));
	}

	if (connected_) {
		std::cerr << "Warning: Connection was still established." << std::endl;
	}
}

void SOCKS5ProxiedConnection::connect(const HostAddressPort& server) {
	server_ = server;
	connection_ = connectionFactory_->createConnection();
	connection_->onConnectFinished.connect(boost::bind(&SOCKS5ProxiedConnection::handleConnectionConnectFinished, shared_from_this(), _1));
	connection_->onDataRead.connect(boost::bind(&SOCKS5ProxiedConnection::handleDataRead, shared_from_this(), _1));
	connection_->onDisconnected.connect(boost::bind(&SOCKS5ProxiedConnection::handleDisconnected, shared_from_this(), _1));
	SWIFT_LOG(debug) << "Trying to connect via proxy " << proxy_.getAddress().toString() << ":" << proxy_.getPort() << std::endl;
	SWIFT_LOG(debug) << "to server " << server.getAddress().toString() << ":" << server.getPort() << std::endl;
	connection_->connect(proxy_);
}

void SOCKS5ProxiedConnection::listen() {
	assert(false);
	connection_->listen();
}

void SOCKS5ProxiedConnection::disconnect() {
	connected_ = false;
	if (connection_) {
		connection_->disconnect();
	}
}

void SOCKS5ProxiedConnection::handleDisconnected(const boost::optional<Error>& error) {
	onDisconnected(error);
}

void SOCKS5ProxiedConnection::write(const SafeByteArray& data) {
	if (connection_) {
		connection_->write(data);
	}
}

void SOCKS5ProxiedConnection::handleConnectionConnectFinished(bool error) {
	connection_->onConnectFinished.disconnect(boost::bind(&SOCKS5ProxiedConnection::handleConnectionConnectFinished, shared_from_this(), _1));
	if (!error) {
		SWIFT_LOG(debug) << "Connection to proxy established, now connect to the server via it." << std::endl;
		
		proxyState_ = ProxyAuthenticating;
		SafeByteArray socksConnect;
		socksConnect.push_back(0x05); // VER = SOCKS5 = 0x05
		socksConnect.push_back(0x01); // Number of authentication methods after this byte.
		socksConnect.push_back(0x00); // 0x00 == no authentication
		// buffer.push_back(0x01); // 0x01 == GSSAPI 
		// buffer.push_back(0x02); // 0x02 ==  Username/Password
		// rest see RFC 1928 (http://tools.ietf.org/html/rfc1928)
		connection_->write(socksConnect);
	}
	else {
		onConnectFinished(true);
	}
}

void SOCKS5ProxiedConnection::handleDataRead(boost::shared_ptr<SafeByteArray> data) {
	SafeByteArray socksConnect;
	boost::asio::ip::address rawAddress = server_.getAddress().getRawAddress();
	assert(rawAddress.is_v4() || rawAddress.is_v6());
	if (!connected_) {
		if (proxyState_ == ProxyAuthenticating) {
			SWIFT_LOG(debug) << "ProxyAuthenticating response received, reply with the connect BYTEs" << std::endl;
			unsigned char choosenMethod = static_cast<unsigned char> ((*data)[1]);
			if ((*data)[0] == 0x05 && choosenMethod != 0xFF) {
				switch(choosenMethod) { // use the correct Method
					case 0x00:
						try {
							proxyState_ = ProxyConnecting;
							socksConnect.push_back(0x05); // VER = SOCKS5 = 0x05
							socksConnect.push_back(0x01); // Construct a TCP connection. (CMD)
							socksConnect.push_back(0x00); // reserved.
							socksConnect.push_back(rawAddress.is_v4() ? 0x01 : 0x04); // IPv4 == 0x01, Hostname == 0x02, IPv6 == 0x04. (ATYP)
							size_t size = rawAddress.is_v4() ? rawAddress.to_v4().to_bytes().size() : rawAddress.to_v6().to_bytes().size();
							for (size_t s = 0; s < size; s++) {
								unsigned char uc;
								if(rawAddress.is_v4()) {
									uc = rawAddress.to_v4().to_bytes()[s]; // the address.
								}
								else {
									uc = rawAddress.to_v6().to_bytes()[s]; // the address.
								}
								socksConnect.push_back(static_cast<char>(uc));
						
							}
							socksConnect.push_back(static_cast<unsigned char> ((server_.getPort() >> 8) & 0xFF)); // highbyte of the port.
							socksConnect.push_back(static_cast<unsigned char> (server_.getPort() & 0xFF)); // lowbyte of the port.
							connection_->write(socksConnect);
							return;
						}
						catch(...) {
							std::cerr << "exception caught" << std::endl;
						}
						connection_->write(socksConnect);
						break;
					default:
						onConnectFinished(true);
						break;
				}
				return;
			}
		}
		else if (proxyState_ == ProxyConnecting) {
			SWIFT_LOG(debug) << "Connect response received, check if successfully." << std::endl;
			SWIFT_LOG(debug) << "Errorbyte: 0x" << std::hex << static_cast<int> ((*data)[1]) << std::dec << std::endl;
			/*

			data.at(1) can be one of the following:
			0x00 	succeeded
			0x01 	general SOCKS server failure
			0x02 	connection not allowed by ruleset
			0x03 	Network unreachable
			0x04 	Host unreachable
			0x05 	Connection refused
			0x06 	TTL expired
			0x07 	Command not supported (CMD)
			0x08 	Address type not supported (ATYP)
			0x09 bis 0xFF 	unassigned
			*/
			if ((*data)[0] == 0x05 && (*data)[1] == 0x0) {
				SWIFT_LOG(debug) << "Successfully connected the server via the proxy." << std::endl;
				connected_ = true;
				onConnectFinished(false);
				return;
			}
			else {
				std::cerr << "SOCKS Proxy returned an error: " << std::hex << (*data)[1] << std::endl;
			}
			return;
		}
	}
	else {
		onDataRead(data);
		return;
	}
	disconnect();
	onConnectFinished(true);
}

HostAddressPort SOCKS5ProxiedConnection::getLocalAddress() const {
	return connection_->getLocalAddress();
}
