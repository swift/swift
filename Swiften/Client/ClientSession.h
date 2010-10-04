/*
 * Copyright (c) 2010 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#pragma once

#include "Swiften/Base/boost_bsignals.h"
#include <boost/shared_ptr.hpp>
#include <boost/enable_shared_from_this.hpp>

#include "Swiften/Base/Error.h"
#include "Swiften/Session/SessionStream.h"
#include "Swiften/Session/BasicSessionStream.h"
#include "Swiften/Base/String.h"
#include "Swiften/JID/JID.h"
#include "Swiften/Elements/Element.h"
#include "Swiften/StreamManagement/StanzaAckRequester.h"
#include "Swiften/StreamManagement/StanzaAckResponder.h"

namespace Swift {
	class ClientAuthenticator;

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
				EnablingSessionManagement,
				BindingResource,
				StartingSession,
				Initialized,
				Finished
			};

			struct Error : public Swift::Error {
				enum Type {
					AuthenticationFailedError,
					CompressionFailedError,
					ServerVerificationFailedError,
					NoSupportedAuthMechanismsError,
					UnexpectedElementError,
					ResourceBindError,
					SessionStartError,
					TLSClientCertificateError,
					TLSError,
				} type;
				Error(Type type) : type(type) {}
			};

			~ClientSession();
			static boost::shared_ptr<ClientSession> create(const JID& jid, boost::shared_ptr<SessionStream> stream) {
				return boost::shared_ptr<ClientSession>(new ClientSession(jid, stream));
			}

			State getState() const {
				return state;
			}

			void setAllowPLAINOverNonTLS(bool b) {
				allowPLAINOverNonTLS = b;
			}

			bool getStreamManagementEnabled() const {
				return stanzaAckRequester_;
			}

			const JID& getLocalJID() const {
				return localJID;
			}

			void start();
			void finish();

			void sendCredentials(const String& password);
			void sendStanza(boost::shared_ptr<Stanza>);

		public:
			boost::signal<void ()> onNeedCredentials;
			boost::signal<void ()> onInitialized;
			boost::signal<void (boost::shared_ptr<Swift::Error>)> onFinished;
			boost::signal<void (boost::shared_ptr<Stanza>)> onStanzaReceived;
			boost::signal<void (boost::shared_ptr<Stanza>)> onStanzaAcked;
		
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

			void handleElement(boost::shared_ptr<Element>);
			void handleStreamStart(const ProtocolHeader&);
			void handleStreamError(boost::shared_ptr<Swift::Error>);

			void handleTLSEncrypted();

			bool checkState(State);
			void continueSessionInitialization();

			void requestAck();
			void handleStanzaAcked(boost::shared_ptr<Stanza> stanza);
			void ack(unsigned int handledStanzasCount);

		private:
			JID localJID;
			State state;
			boost::shared_ptr<SessionStream> stream;
			bool allowPLAINOverNonTLS;
			bool needSessionStart;
			bool needResourceBind;
			bool needAcking;
			ClientAuthenticator* authenticator;
			boost::shared_ptr<StanzaAckRequester> stanzaAckRequester_;
			boost::shared_ptr<StanzaAckResponder> stanzaAckResponder_;
			boost::bsignals::connection streamOnStreamStartReceivedConnection;
			boost::bsignals::connection streamOnElementReceivedConnection;
			boost::bsignals::connection streamOnErrorConnection;
			boost::bsignals::connection streamOnTLSEncryptedConnection;
	};
}
