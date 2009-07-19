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

	class Session : public boost::enable_shared_from_this<Session> {
		public:
			enum Error {
				ConnectionError,
				XMLError
			};

			Session(
					boost::shared_ptr<Connection> connection,
					PayloadParserFactoryCollection* payloadParserFactories, 
					PayloadSerializerCollection* payloadSerializers);
			virtual ~Session();

			void startSession();
			void finishSession();
			void sendStanza(boost::shared_ptr<Stanza>);

			boost::signal<void (boost::shared_ptr<Stanza>)> onStanzaReceived;
			boost::signal<void ()> onSessionStarted;
			boost::signal<void (const boost::optional<Error>&)> onSessionFinished;
			boost::signal<void (const ByteArray&)> onDataWritten;
			boost::signal<void (const ByteArray&)> onDataRead;

		protected:
			void finishSession(const Error&);

			virtual void handleSessionStarted() {}
			virtual void handleElement(boost::shared_ptr<Element>) = 0;
			virtual void handleStreamStart(const ProtocolHeader&) = 0;

			void initializeStreamStack();

			boost::shared_ptr<XMPPLayer> getXMPPLayer() const {
				return xmppLayer;
			}

			void setInitialized();
			bool isInitialized() const { 
				return initialized; 
			}

		private:
			void handleDisconnected(const boost::optional<Connection::Error>& error);

		private:
			boost::shared_ptr<Connection> connection;
			PayloadParserFactoryCollection* payloadParserFactories;
			PayloadSerializerCollection* payloadSerializers;
			boost::shared_ptr<XMPPLayer> xmppLayer;
			boost::shared_ptr<ConnectionLayer> connectionLayer;
			StreamStack* streamStack;
			bool initialized;
	};
}
