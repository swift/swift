#pragma once

#include <boost/shared_ptr.hpp>
#include <boost/signal.hpp>

#include "Swiften/LinkLocal/LinkLocalSession.h"
#include "Swiften/JID/JID.h"
#include "Swiften/Network/Connection.h"

namespace Swift {
	class ProtocolHeader;
	class String;
	class Element;
	class PayloadParserFactoryCollection;
	class PayloadSerializerCollection;

	class IncomingLinkLocalSession : public LinkLocalSession {
		public:
			IncomingLinkLocalSession(
					const JID& localJID,
					boost::shared_ptr<Connection> connection, 
					PayloadParserFactoryCollection* payloadParserFactories, 
					PayloadSerializerCollection* payloadSerializers);

			const JID& getRemoteJID() const {
				return remoteJID_;
			}

			void start();

		private:
			void handleElement(boost::shared_ptr<Element>);
			void handleStreamStart(const ProtocolHeader&);

		private:
			bool initialized_;
			JID remoteJID_;
	};
}
