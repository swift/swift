#pragma once

#include <boost/signal.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/enable_shared_from_this.hpp>

#include "Swiften/Base/Error.h"
#include "Swiften/Session/SessionStream.h"
#include "Swiften/Session/BasicSessionStream.h"
#include "Swiften/Base/String.h"
#include "Swiften/JID/JID.h"
#include "Swiften/Elements/Element.h"

namespace Swift {
	class ClientSession : public boost::enable_shared_from_this<ClientSession> {
		public:
			enum State {
				Initial,
				WaitingForStreamStart,
				Negotiating,
				Compressing,
				WaitingForEncrypt,
				Encrypting,
				WaitingForCredentials,
				Authenticating,
				BindingResource,
				StartingSession,
				Initialized,
				Finished
			};

			struct Error : public Swift::Error {
				enum Type {
					AuthenticationFailedError,
					NoSupportedAuthMechanismsError,
					UnexpectedElementError,
					ResourceBindError,
					SessionStartError,
					TLSClientCertificateError,
					TLSError,
				} type;
				Error(Type type) : type(type) {}
			};

			static boost::shared_ptr<ClientSession> create(const JID& jid, boost::shared_ptr<SessionStream> stream) {
				return boost::shared_ptr<ClientSession>(new ClientSession(jid, stream));
			}

			State getState() const {
				return state;
			}

			void start();
			void finish();

			void sendCredentials(const String& password);
			void sendElement(boost::shared_ptr<Element> element);

		public:
			boost::signal<void ()> onNeedCredentials;
			boost::signal<void ()> onInitialized;
			boost::signal<void (boost::shared_ptr<Swift::Error>)> onFinished;
			boost::signal<void (boost::shared_ptr<Element>)> onElementReceived;
		
		private:
			ClientSession(
					const JID& jid, 
					boost::shared_ptr<SessionStream>);

			void finishSession(Error::Type error);
			void finishSession(boost::shared_ptr<Swift::Error> error);

			JID getRemoteJID() const {
				return JID("", localJID.getDomain());
			}

			void sendStreamHeader();
			void sendSessionStart();

			void handleElement(boost::shared_ptr<Element>);
			void handleStreamStart(const ProtocolHeader&);
			void handleStreamError(boost::shared_ptr<Swift::Error>);

			void handleTLSEncrypted();

			bool checkState(State);

		private:
			JID localJID;
			State state;
			boost::shared_ptr<SessionStream> stream;
			bool needSessionStart;
	};
}
