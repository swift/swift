/*
 * Copyright (c) 2010-2011 Thilo Cestonaro
 * Licensed under the simplified BSD license.
 * See Documentation/Licenses/BSD-simplified.txt for more information.
 */

/*
 * Copyright (c) 2011-2012 Kevin Smith
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */


#include <Swiften/Network/HTTPConnectProxiedConnection.h>

#include <iostream>
#include <boost/bind.hpp>
#include <boost/lexical_cast.hpp>

#include <Swiften/Base/Algorithm.h>
#include <Swiften/Base/Log.h>
#include <Swiften/Base/String.h>
#include <Swiften/Base/ByteArray.h>
#include <Swiften/Network/HostAddressPort.h>
#include <Swiften/Network/ConnectionFactory.h>
#include <Swiften/StringCodecs/Base64.h>

using namespace Swift;

HTTPConnectProxiedConnection::HTTPConnectProxiedConnection(
		DomainNameResolver* resolver, 
		ConnectionFactory* connectionFactory, 
		TimerFactory* timerFactory, 
		const std::string& proxyHost, 
		int proxyPort, 
		const SafeString& authID, 
		const SafeString& authPassword) : 
			ProxiedConnection(resolver, connectionFactory, timerFactory, proxyHost, proxyPort),
			authID_(authID), 
			authPassword_(authPassword) {
}


void HTTPConnectProxiedConnection::initializeProxy() {
	std::stringstream connect;
	connect << "CONNECT " << getServer().getAddress().toString() << ":" << getServer().getPort() << " HTTP/1.1\r\n";
	SafeByteArray data = createSafeByteArray(connect.str());
	if (!authID_.empty() && !authPassword_.empty()) {
		append(data, createSafeByteArray("Proxy-Authorization: Basic "));
		SafeByteArray credentials = authID_;
		append(credentials, createSafeByteArray(":"));
		append(credentials, authPassword_);
		append(data, Base64::encode(credentials));
		append(data, createSafeByteArray("\r\n"));
	}
	append(data, createSafeByteArray("\r\n"));
	SWIFT_LOG(debug) << "HTTP Proxy send headers: " << byteArrayToString(ByteArray(data.begin(), data.end())) << std::endl;
	write(data);
}

void HTTPConnectProxiedConnection::handleProxyInitializeData(boost::shared_ptr<SafeByteArray> data) {
	SWIFT_LOG(debug) << byteArrayToString(ByteArray(data->begin(), data->end())) << std::endl;
	std::vector<std::string> tmp = String::split(byteArrayToString(ByteArray(data->begin(), data->end())), ' ');
	if (tmp.size() > 1) {
		try {
			int status = boost::lexical_cast<int>(tmp[1]);
			SWIFT_LOG(debug) << "Proxy Status: " << status << std::endl;
			if (status / 100 == 2) { // all 2XX states are OK
				setProxyInitializeFinished(true);
			}
			else {
				SWIFT_LOG(debug) << "HTTP Proxy returned an error: " << byteArrayToString(ByteArray(data->begin(), data->end())) << std::endl;
				setProxyInitializeFinished(false);
			}
		}
		catch (boost::bad_lexical_cast&) {
			SWIFT_LOG(warning) << "Unexpected response: " << tmp[1] << std::endl;
			setProxyInitializeFinished(false);
		}
	}
	else {
		setProxyInitializeFinished(false);
	}
}
