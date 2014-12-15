/*
 * Copyright (c) 2010-2014 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <boost/shared_ptr.hpp>
#include <Swiften/Base/boost_bsignals.h>
#include <boost/enable_shared_from_this.hpp>
#include <vector>

#include <Swiften/Base/API.h>
#include <Swiften/Network/Connection.h>
#include <Swiften/LinkLocal/LinkLocalService.h>

namespace Swift {
	class ConnectionFactory;
	class HostAddress;
	class ToplevelElement;
	class PayloadParserFactoryCollection;
	class PayloadSerializerCollection;
	class DNSSDQuerier;
	class DNSSDResolveHostnameQuery;

	class SWIFTEN_API LinkLocalConnector : public boost::enable_shared_from_this<LinkLocalConnector> {
		public:
			LinkLocalConnector(
					const LinkLocalService& service, 
					boost::shared_ptr<DNSSDQuerier> querier,
					boost::shared_ptr<Connection> connection);
			~LinkLocalConnector();

			const LinkLocalService& getService() const {
				return service;
			}

			void connect();
			void cancel();
			void queueElement(boost::shared_ptr<ToplevelElement> element);

			const std::vector<boost::shared_ptr<ToplevelElement> >& getQueuedElements() const {
				return queuedElements;
			}

			boost::shared_ptr<Connection> getConnection() const {
				return connection;
			}

			boost::signal<void (bool)> onConnectFinished;

		private:
			void handleHostnameResolved(const boost::optional<HostAddress>& address);
			void handleConnected(bool error);

		private:
			LinkLocalService service;
			boost::shared_ptr<DNSSDQuerier> querier;
			boost::shared_ptr<DNSSDResolveHostnameQuery> resolveQuery;
			boost::bsignals::connection resolveQueryHostNameResolvedConnection;
			boost::shared_ptr<Connection> connection;
			boost::bsignals::connection connectionConnectFinishedConnection;
			std::vector<boost::shared_ptr<ToplevelElement> > queuedElements;
	};
}
