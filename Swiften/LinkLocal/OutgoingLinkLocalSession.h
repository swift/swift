#pragma once

#include <boost/shared_ptr.hpp>
#include <boost/signal.hpp>
#include <boost/enable_shared_from_this.hpp>
#include <vector>

#include "Swiften/LinkLocal/LinkLocalSession.h"
#include "Swiften/JID/JID.h"

namespace Swift {
	class ConnectionFactory;
	class HostAddress;
	class String;
	class Element;
	class PayloadParserFactoryCollection;
	class PayloadSerializerCollection;
	class DNSSDService;

	class OutgoingLinkLocalSession : public LinkLocalSession {
		public:
			OutgoingLinkLocalSession(
					const JID& localJID,
					const JID& remoteJID,
					const String& hostname,
					int port,
					boost::shared_ptr<DNSSDService> resolver,
					PayloadParserFactoryCollection* payloadParserFactories, 
					PayloadSerializerCollection* payloadSerializers,
					ConnectionFactory* connectionFactory);

			const JID& getRemoteJID() const {
				return remoteJID_;
			}

			void start();

			void sendStanza(boost::shared_ptr<Stanza> stanza);

		private:
			void handleElement(boost::shared_ptr<Element>);
			void handleStreamStart(const ProtocolHeader&);
			void handleHostnameResolved(const String& hostname, const boost::optional<HostAddress>& address);
			void handleConnected();

		private:
			bool resolving_;
			JID remoteJID_;
			String hostname_;
			int port_;
			boost::shared_ptr<DNSSDService> resolver_;
			std::vector<boost::shared_ptr<Stanza> > queuedStanzas_;
			ConnectionFactory* connectionFactory_;
	};
}
