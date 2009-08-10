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
	class Element;
	class ByteArray;
	class PayloadParserFactoryCollection;
	class PayloadSerializerCollection;
	class XMPPLayer;

	class Session : public boost::enable_shared_from_this<Session> {
		public:
			enum SessionError {
				ConnectionReadError,
				ConnectionWriteError,
				XMLError,
				AuthenticationFailedError,
				NoSupportedAuthMechanismsError,
				UnexpectedElementError,
				ResourceBindError,
				SessionStartError,
				TLSError,
				ClientCertificateLoadError,
				ClientCertificateError
			};

			Session(
					boost::shared_ptr<Connection> connection,
					PayloadParserFactoryCollection* payloadParserFactories, 
					PayloadSerializerCollection* payloadSerializers);
			virtual ~Session();

			void startSession();
			void finishSession();
			void sendElement(boost::shared_ptr<Element>);

			const JID& getLocalJID() const {
				return localJID;
			}

			const JID& getRemoteJID() const {
				return remoteJID;
			}

			boost::signal<void (boost::shared_ptr<Element>)> onElementReceived;
			boost::signal<void ()> onSessionStarted;
			boost::signal<void (const boost::optional<SessionError>&)> onSessionFinished;
			boost::signal<void (const ByteArray&)> onDataWritten;
			boost::signal<void (const ByteArray&)> onDataRead;

		protected:
			void setRemoteJID(const JID& j) {
				assert(!isInitialized());
				remoteJID = j;
			}

			void setLocalJID(const JID& j) {
				assert(!isInitialized());
				localJID = j;
			}

			void finishSession(const SessionError&);

			virtual void handleSessionStarted() {}
			virtual void handleSessionFinished(const boost::optional<SessionError>&) {}
			virtual void handleElement(boost::shared_ptr<Element>) = 0;
			virtual void handleStreamStart(const ProtocolHeader&) = 0;

			void initializeStreamStack();

			boost::shared_ptr<XMPPLayer> getXMPPLayer() const {
				return xmppLayer;
			}

			StreamStack* getStreamStack() const {
				return streamStack;
			}

			void setInitialized();
			bool isInitialized() const { 
				return initialized; 
			}

			void setFinished();

		private:
			void handleDisconnected(const boost::optional<Connection::Error>& error);

		private:
			JID localJID;
			JID remoteJID;
			boost::shared_ptr<Connection> connection;
			PayloadParserFactoryCollection* payloadParserFactories;
			PayloadSerializerCollection* payloadSerializers;
			boost::shared_ptr<XMPPLayer> xmppLayer;
			boost::shared_ptr<ConnectionLayer> connectionLayer;
			StreamStack* streamStack;
			bool initialized;
			bool finishing;
	};
}
