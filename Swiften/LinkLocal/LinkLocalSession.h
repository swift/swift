#pragma once

#include <boost/shared_ptr.hpp>
#include <boost/signal.hpp>
#include <boost/optional.hpp>
#include <boost/enable_shared_from_this.hpp>

#include "Swiften/JID/JID.h"
#include "Swiften/Elements/Element.h"
#include "Swiften/Network/Connection.h"
#include "Swiften/StreamStack/ConnectionLayer.h"

namespace Swift {
	class ProtocolHeader;
	class StreamStack;
	class JID;
	class Stanza;
	class ByteArray;
	class PayloadParserFactoryCollection;
	class PayloadSerializerCollection;
	class XMPPLayer;

	class LinkLocalSession : 
			public boost::enable_shared_from_this<LinkLocalSession> {
		public:
			enum Error { ConnectionError, UnknownError };

			LinkLocalSession(
					const JID& localJID,
					boost::shared_ptr<Connection> connection,
					PayloadParserFactoryCollection* payloadParserFactories, 
					PayloadSerializerCollection* payloadSerializers);
			virtual ~LinkLocalSession();

			void finishSession();

			// TODO: Make non-virtual when OutgoingSession is fixed
			virtual void sendStanza(boost::shared_ptr<Stanza>);

			virtual const JID& getRemoteJID() const = 0;

			virtual void start() = 0;

			boost::signal<void (boost::shared_ptr<Stanza>)> onStanzaReceived;
			boost::signal<void (boost::optional<Error>)> onSessionFinished;
			boost::signal<void ()> onSessionStarted;
			boost::signal<void (const ByteArray&)> onDataWritten;
			boost::signal<void (const ByteArray&)> onDataRead;

		protected:
			void initializeStreamStack();

			const JID& getLocalJID() const {
				return localJID;
			}

			boost::shared_ptr<XMPPLayer> getXMPPLayer() const {
				return xmppLayer;
			}

			// TODO: Remove later
			void setConnection(boost::shared_ptr<Connection> c) {
				connection = c;
			}

			virtual void handleElement(boost::shared_ptr<Element>) = 0;
			virtual void handleStreamStart(const ProtocolHeader&) = 0;

			void setInitialized();
			bool isInitialized() const { return initialized; }

		private:
			void handleDisconnected(const boost::optional<Connection::Error>& error);

		private:
			JID localJID;
			PayloadParserFactoryCollection* payloadParserFactories;
			PayloadSerializerCollection* payloadSerializers;
			boost::shared_ptr<Connection> connection;
			boost::shared_ptr<XMPPLayer> xmppLayer;
			boost::shared_ptr<ConnectionLayer> connectionLayer;
			StreamStack* streamStack;
			bool initialized;
	};
}
