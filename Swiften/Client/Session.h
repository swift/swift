#ifndef SWIFTEN_Session_H
#define SWIFTEN_Session_H

#include <boost/signal.hpp>
#include <boost/shared_ptr.hpp>

#include "Swiften/Base/String.h"
#include "Swiften/JID/JID.h"
#include "Swiften/Elements/Element.h"
#include "Swiften/Network/Connection.h"
#include "Swiften/TLS/PKCS12Certificate.h"

namespace Swift {
	class PayloadParserFactoryCollection;
	class PayloadSerializerCollection;
	class ConnectionFactory;
	class Connection;
	class StreamStack;
	class XMPPLayer;
	class ConnectionLayer;
	class TLSLayerFactory;
	class TLSLayer;
	class WhitespacePingLayer;

	class Session {
		public:
			enum State {
				Initial,
				Connecting,
				WaitingForStreamStart,
				Negotiating,
				Compressing,
				Encrypting,
				WaitingForCredentials,
				Authenticating,
				BindingResource,
				StartingSession,
				SessionStarted,
				Error
			};
			enum SessionError {
				NoError,
				DomainNameResolveError,
				ConnectionError,
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

			Session(const JID& jid, ConnectionFactory*, TLSLayerFactory*, PayloadParserFactoryCollection*, PayloadSerializerCollection*);
			~Session();

			State getState() const {
				return state_;
			}

			SessionError getError() const {
				return error_;
			}

			const JID& getJID() const {
				return jid_;
			}

			void start();
			void stop();
			void sendCredentials(const String& password);
			void sendElement(boost::shared_ptr<Element>);
			void setCertificate(const PKCS12Certificate& certificate);

		protected:
			StreamStack* getStreamStack() const {
				return streamStack_;
			}

		private:
			void initializeStreamStack();
			void sendStreamHeader();
			void sendSessionStart();

			void handleConnected();
			void handleConnectionError(Connection::Error);
			void handleElement(boost::shared_ptr<Element>);
			void handleStreamStart();
			void handleTLSConnected();
			void handleTLSError();

			void setError(SessionError);
			bool checkState(State);

		public:
			boost::signal<void ()> onSessionStarted;
			boost::signal<void (SessionError)> onError;
			boost::signal<void ()> onNeedCredentials;
			boost::signal<void (boost::shared_ptr<Element>) > onElementReceived;
			boost::signal<void (const ByteArray&)> onDataWritten;
			boost::signal<void (const ByteArray&)> onDataRead;
		
		private:
			JID jid_;
			ConnectionFactory* connectionFactory_;
			TLSLayerFactory* tlsLayerFactory_;
			PayloadParserFactoryCollection* payloadParserFactories_;
			PayloadSerializerCollection* payloadSerializers_;
			State state_;
			SessionError error_;
			boost::shared_ptr<Connection> connection_;
			XMPPLayer* xmppLayer_;
			TLSLayer* tlsLayer_;
			ConnectionLayer* connectionLayer_;
			WhitespacePingLayer* whitespacePingLayer_;
			StreamStack* streamStack_;
			bool needSessionStart_;
			PKCS12Certificate certificate_;
	};

}

#endif
