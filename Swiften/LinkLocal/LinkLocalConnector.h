#pragma once

#include <boost/shared_ptr.hpp>
#include <boost/signal.hpp>
#include <boost/enable_shared_from_this.hpp>
#include <vector>

#include "Swiften/JID/JID.h"
#include "Swiften/Network/Connection.h"

namespace Swift {
	class ConnectionFactory;
	class HostAddress;
	class String;
	class Element;
	class PayloadParserFactoryCollection;
	class PayloadSerializerCollection;
	class DNSSDQuerier;

	class LinkLocalConnector : public boost::enable_shared_from_this<LinkLocalConnector> {
		public:
			LinkLocalConnector(
					const JID& remoteJID,
					const String& hostname,
					int port,
					boost::shared_ptr<DNSSDQuerier> querier,
					boost::shared_ptr<Connection> connection);

			const JID& getRemoteJID() const {
				return remoteJID_;
			}

			void connect();
			void queueElement(boost::shared_ptr<Element> element);

			const std::vector<boost::shared_ptr<Element> >& getQueuedElements() const {
				return queuedElements_;
			}

			boost::shared_ptr<Connection> getConnection() const {
				return connection_;
			}

			boost::signal<void (bool)> onConnectFinished;

		private:
			//void handleHostnameResolved(const String& hostname, const boost::optional<HostAddress>& address);
			void handleConnected(bool error);

		private:
			JID remoteJID_;
			String hostname_;
			int port_;
			boost::shared_ptr<DNSSDQuerier> querier_;
			boost::shared_ptr<Connection> connection_;
			bool resolving_;
			std::vector<boost::shared_ptr<Element> > queuedElements_;
	};
}
