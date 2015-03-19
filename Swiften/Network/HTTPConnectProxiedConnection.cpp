/*
 * Copyright (c) 2010-2011 Thilo Cestonaro
 * Licensed under the simplified BSD license.
 * See Documentation/Licenses/BSD-simplified.txt for more information.
 */

/*
 * Copyright (c) 2011-2015 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */


#include <Swiften/Network/HTTPConnectProxiedConnection.h>

#include <iostream>
#include <utility>

#include <boost/bind.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/algorithm/string.hpp>

#include <Swiften/Base/foreach.h>
#include <Swiften/Base/Algorithm.h>
#include <Swiften/Base/Log.h>
#include <Swiften/Base/String.h>
#include <Swiften/Base/ByteArray.h>
#include <Swiften/Network/HostAddressPort.h>
#include <Swiften/Network/ConnectionFactory.h>
#include <Swiften/Network/HTTPTrafficFilter.h>
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

void HTTPConnectProxiedConnection::setHTTPTrafficFilter(boost::shared_ptr<HTTPTrafficFilter> trafficFilter) {
	trafficFilter_ = trafficFilter;
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

void HTTPConnectProxiedConnection::parseHTTPHeader(const std::string& data, std::string& statusLine, std::vector<std::pair<std::string, std::string> >& headerFields) {
	std::istringstream dataStream(data);

	// parse status line
	std::getline(dataStream, statusLine);

	// parse fields
	std::string headerLine;
	std::string::size_type splitIndex;
	while (std::getline(dataStream, headerLine) && headerLine != "\r") {
		splitIndex = headerLine.find(':', 0);
		if (splitIndex != std::string::npos) {
			headerFields.push_back(std::pair<std::string, std::string>(headerLine.substr(0, splitIndex), headerLine.substr(splitIndex + 1)));
		}
	}
}

void HTTPConnectProxiedConnection::sendHTTPRequest(const std::string& statusLine, std::vector<std::pair<std::string, std::string> >& headerFields) {
	typedef std::pair<std::string, std::string> HTTPHeaderField;
	std::stringstream request;

	request << statusLine << "\r\n";
	foreach (const HTTPHeaderField& field, headerFields) {
		request << field.first << ":" << field.second << "\r\n";
	}
	request << "\r\n";
	write(createSafeByteArray(request.str()));
}

void HTTPConnectProxiedConnection::handleProxyInitializeData(boost::shared_ptr<SafeByteArray> data) {
	std::string dataString = byteArrayToString(ByteArray(data->begin(), data->end()));
	SWIFT_LOG(debug) << data << std::endl;

	std::string statusLine;
	std::vector<std::pair<std::string, std::string> > headerFields;

	std::string::size_type headerEnd = dataString.find("\r\n\r\n", 0);

	parseHTTPHeader(dataString.substr(0, headerEnd), statusLine, headerFields);

	if (trafficFilter_) {
		std::vector<std::pair<std::string, std::string> > newHeaderFields = trafficFilter_->filterHTTPResponseHeader(headerFields);
		if (!newHeaderFields.empty()) {
			std::stringstream statusLine;
			statusLine << "CONNECT " << getServer().getAddress().toString() << ":" << getServer().getPort();
			sendHTTPRequest(statusLine.str(), newHeaderFields);
			SWIFT_LOG(debug) << "send HTTP request from traffic filter" << std::endl;
			return;
		}
	}

	std::vector<std::string> tmp = String::split(statusLine, ' ');
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
