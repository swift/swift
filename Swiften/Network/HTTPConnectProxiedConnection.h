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


#pragma once

#include <boost/shared_ptr.hpp>

#include <Swiften/Base/API.h>
#include <Swiften/Network/ProxiedConnection.h>

namespace Swift {
	class ConnectionFactory;
	class DomainNameResolver;
	class EventLoop;
	class HTTPTrafficFilter;
	class TimerFactory;

	class SWIFTEN_API HTTPConnectProxiedConnection : public ProxiedConnection {
		public:
			typedef boost::shared_ptr<HTTPConnectProxiedConnection> ref;

			virtual ~HTTPConnectProxiedConnection();

			static ref create(DomainNameResolver* resolver, ConnectionFactory* connectionFactory, TimerFactory* timerFactory, const std::string& proxyHost, int proxyPort, const SafeString& authID, const SafeString& authPassword) {
				return ref(new HTTPConnectProxiedConnection(resolver, connectionFactory, timerFactory, proxyHost, proxyPort, authID, authPassword));
			}

			void setHTTPTrafficFilter(boost::shared_ptr<HTTPTrafficFilter> trafficFilter);

		private:
			HTTPConnectProxiedConnection(DomainNameResolver* resolver, ConnectionFactory* connectionFactory, TimerFactory* timerFactory, const std::string& proxyHost, int proxyPort, const SafeString& authID, const SafeString& authPassword);

			virtual void initializeProxy();
			virtual void handleProxyInitializeData(boost::shared_ptr<SafeByteArray> data);

			void sendHTTPRequest(const std::string& statusLine, const std::vector<std::pair<std::string, std::string> >& headerFields);
			void parseHTTPHeader(const std::string& data, std::string& statusLine, std::vector<std::pair<std::string, std::string> >& headerFields);

		private:
			SafeByteArray authID_;
			SafeByteArray authPassword_;
			boost::shared_ptr<HTTPTrafficFilter> trafficFilter_;
			std::string httpResponseBuffer_;
			std::vector<std::pair<std::string, std::string> > nextHTTPRequestHeaders_;
	};
}
