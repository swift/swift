#pragma once

#include <boost/signal.hpp>
#include <boost/shared_ptr.hpp>

#include "Swiften/Session/Session.h"
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

	class ClientSession : public Session {
		public:
			enum State {
				Initial,
				WaitingForStreamStart,
				Negotiating,
				Compressing,
				Encrypting,
				WaitingForCredentials,
				Authenticating,
				BindingResource,
				StartingSession,
				SessionStarted,
				Error,
				Finished
			};

			ClientSession(
					const JID& jid, 
					boost::shared_ptr<Connection>, 
					TLSLayerFactory*, 
					PayloadParserFactoryCollection*, 
					PayloadSerializerCollection*);

			State getState() const {
				return state_;
			}

			boost::optional<SessionError> getError() const {
				return error_;
			}

			const JID& getJID() const {
				return jid_;
			}

			void sendCredentials(const String& password);
			void setCertificate(const PKCS12Certificate& certificate);

		private:
			void sendStreamHeader();
			void sendSessionStart();

			virtual void handleSessionStarted();
			virtual void handleSessionFinished(const boost::optional<SessionError>& error);
			virtual void handleElement(boost::shared_ptr<Element>);
			virtual void handleStreamStart(const ProtocolHeader&);

			void handleTLSConnected();
			void handleTLSError();

			void setError(SessionError);
			bool checkState(State);

		public:
			boost::signal<void ()> onNeedCredentials;
		
		private:
			JID jid_;
			TLSLayerFactory* tlsLayerFactory_;
			State state_;
			boost::optional<SessionError> error_;
			boost::shared_ptr<TLSLayer> tlsLayer_;
			boost::shared_ptr<WhitespacePingLayer> whitespacePingLayer_;
			bool needSessionStart_;
			PKCS12Certificate certificate_;
	};
}
