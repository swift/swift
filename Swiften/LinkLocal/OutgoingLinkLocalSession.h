#pragma once

#include <boost/shared_ptr.hpp>
#include <boost/signal.hpp>
#include <boost/enable_shared_from_this.hpp>
#include <vector>

#include "Swiften/Session/Session.h"
#include "Swiften/JID/JID.h"

namespace Swift {
	class ConnectionFactory;
	class String;
	class Element;
	class PayloadParserFactoryCollection;
	class PayloadSerializerCollection;

	class OutgoingLinkLocalSession : public Session {
		public:
			OutgoingLinkLocalSession(
					const JID& localJID,
					const JID& remoteJID,
					boost::shared_ptr<Connection> connection,
					PayloadParserFactoryCollection* payloadParserFactories, 
					PayloadSerializerCollection* payloadSerializers);

			void queueElement(boost::shared_ptr<Element> element);

		private:
			void handleElement(boost::shared_ptr<Element>);
			void handleStreamStart(const ProtocolHeader&);

		private:
			std::vector<boost::shared_ptr<Element> > queuedElements_;
	};
}
