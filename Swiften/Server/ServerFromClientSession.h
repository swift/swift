#pragma once

#include <boost/shared_ptr.hpp>
#include <boost/signal.hpp>
#include <boost/enable_shared_from_this.hpp>

#include "Swiften/Base/String.h"
#include "Swiften/JID/JID.h"
#include "Swiften/Network/Connection.h"

namespace Swift {
	class ProtocolHeader;
	class Element;
	class Stanza;
	class PayloadParserFactoryCollection;
	class PayloadSerializerCollection;
	class StreamStack;
	class UserRegistry;
	class XMPPLayer;
	class ConnectionLayer;
	class Connection;
	class ByteArray;

	class ServerFromClientSession : public boost::enable_shared_from_this<ServerFromClientSession> {
		public:
			ServerFromClientSession(
					const String& id,
					boost::shared_ptr<Connection> connection, 
					PayloadParserFactoryCollection* payloadParserFactories, 
					PayloadSerializerCollection* payloadSerializers,
					UserRegistry* userRegistry);
			~ServerFromClientSession();

			void start();

			void sendStanza(boost::shared_ptr<Stanza>);

			const JID& getJID() const {
				return jid_;
			}

			const JID& getDomain() const {
				return domain_;
			}

			boost::signal<void (boost::shared_ptr<Stanza>)> onStanzaReceived;
			boost::signal<void ()> onSessionFinished;
			boost::signal<void ()> onSessionStarted;
			boost::signal<void (const ByteArray&)> onDataWritten;
			boost::signal<void (const ByteArray&)> onDataRead;

		private:
			void handleDisconnected(const boost::optional<Connection::Error>& error);
			void handleElement(boost::shared_ptr<Element>);
			void handleStreamStart(const ProtocolHeader& header);

		private:
			String id_;
			boost::shared_ptr<Connection> connection_;
			PayloadParserFactoryCollection* payloadParserFactories_;
			PayloadSerializerCollection* payloadSerializers_;
			UserRegistry* userRegistry_;
			bool authenticated_;
			bool initialized_;
			boost::shared_ptr<XMPPLayer> xmppLayer_;
			boost::shared_ptr<ConnectionLayer> connectionLayer_;
			StreamStack* streamStack_;
			JID domain_;
			String user_;
			JID jid_;
	};
}
