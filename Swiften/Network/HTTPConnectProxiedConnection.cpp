/*
 * Copyright (c) 2010-2011 Thilo Cestonaro
 * Licensed under the simplified BSD license.
 * See Documentation/Licenses/BSD-simplified.txt for more information.
 */

/*
 * Copyright (c) 2011-2016 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */


#include <Swiften/Network/HTTPConnectProxiedConnection.h>

#include <iostream>
#include <utility>

#include <boost/algorithm/string.hpp>
#include <boost/bind.hpp>
#include <boost/lexical_cast.hpp>

#include <Swiften/Base/Algorithm.h>
#include <Swiften/Base/ByteArray.h>
#include <Swiften/Base/Log.h>
#include <Swiften/Base/String.h>
#include <Swiften/Network/ConnectionFactory.h>
#include <Swiften/Network/HTTPTrafficFilter.h>
#include <Swiften/Network/HostAddressPort.h>
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

HTTPConnectProxiedConnection::~HTTPConnectProxiedConnection() {

}

void HTTPConnectProxiedConnection::setHTTPTrafficFilter(std::shared_ptr<HTTPTrafficFilter> trafficFilter) {
    trafficFilter_ = trafficFilter;
}

void HTTPConnectProxiedConnection::initializeProxy() {
    httpResponseBuffer_.clear();

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
    else if (!nextHTTPRequestHeaders_.empty()) {
        for (const auto& headerField : nextHTTPRequestHeaders_) {
            append(data, createSafeByteArray(headerField.first));
            append(data, createSafeByteArray(": "));
            append(data, createSafeByteArray(headerField.second));
            append(data, createSafeByteArray("\r\n"));
        }

        nextHTTPRequestHeaders_.clear();
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

void HTTPConnectProxiedConnection::sendHTTPRequest(const std::string& statusLine, const std::vector<std::pair<std::string, std::string> >& headerFields) {
    std::stringstream request;

    request << statusLine << "\r\n";
    for (const auto& field : headerFields) {
        request << field.first << ":" << field.second << "\r\n";
    }
    request << "\r\n";
    write(createSafeByteArray(request.str()));
}

void HTTPConnectProxiedConnection::handleProxyInitializeData(std::shared_ptr<SafeByteArray> data) {
    std::string dataString = byteArrayToString(ByteArray(data->begin(), data->end()));
    SWIFT_LOG(debug) << data << std::endl;
    httpResponseBuffer_.append(dataString);

    std::string statusLine;
    std::vector<std::pair<std::string, std::string> > headerFields;

    std::string::size_type headerEnd = httpResponseBuffer_.find("\r\n\r\n", 0);
    if (headerEnd == std::string::npos) {
        if ((httpResponseBuffer_.size() > 4) && (httpResponseBuffer_.substr(0, 4) != "HTTP")) {
            setProxyInitializeFinished(false);
        }
        return;
    }

    parseHTTPHeader(httpResponseBuffer_.substr(0, headerEnd), statusLine, headerFields);

    if (trafficFilter_) {
        std::vector<std::pair<std::string, std::string> > newHeaderFields = trafficFilter_->filterHTTPResponseHeader(statusLine, headerFields);
        if (!newHeaderFields.empty()) {
            std::stringstream statusLine;
            reconnect();
            nextHTTPRequestHeaders_ = newHeaderFields;
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
                SWIFT_LOG(debug) << "HTTP Proxy returned an error: " << httpResponseBuffer_ << std::endl;
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
    httpResponseBuffer_.clear();
}
