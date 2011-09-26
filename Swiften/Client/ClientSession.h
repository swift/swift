/*
 * Copyright (c) 2010 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#pragma once

#include <Swiften/Base/boost_bsignals.h>
#include <boost/shared_ptr.hpp>
#include <boost/enable_shared_from_this.hpp>

#include <Swiften/Base/Error.h>
#include <Swiften/Session/SessionStream.h>
#include <string>
#include <Swiften/JID/JID.h>
#include <Swiften/Elements/Element.h>
#include <Swiften/StreamManagement/StanzaAckRequester.h>
#include <Swiften/StreamManagement/StanzaAckResponder.h>

namespace Swift {
	class ClientAuthenticator;
	class CertificateTrustChecker;

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
				Finishing,
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
					StreamError,
				} type;
				Error(Type type) : type(type) {}
			};

			enum UseTLS {
				NeverUseTLS,
				UseTLSWhenAvailable,
				RequireTLS
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

			void setUseStreamCompression(bool b) {
				useStreamCompression = b;
			}

			void setUseTLS(UseTLS b) {
				useTLS = b;
			}

			void setUseAcks(bool b) {
				useAcks = b;
			}


			bool getStreamManagementEnabled() const {
				return stanzaAckRequester_;
			}

			bool getRosterVersioningSupported() const {
				return rosterVersioningSupported;
			}

			const JID& getLocalJID() const {
				return localJID;
			}

			void start();
			void finish();

			bool isFinished() const {
				return getState() == Finished;
			}

			void sendCredentials(const SafeByteArray& password);
			void sendStanza(boost::shared_ptr<Stanza>);

			void setCertificateTrustChecker(CertificateTrustChecker* checker) {
				certificateTrustChecker = checker;
			}

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
			void handleStreamClosed(boost::shared_ptr<Swift::Error>);

			void handleTLSEncrypted();

			bool checkState(State);
			void continueSessionInitialization();

			void requestAck();
			void handleStanzaAcked(boost::shared_ptr<Stanza> stanza);
			void ack(unsigned int handledStanzasCount);
			void continueAfterTLSEncrypted();
			void checkTrustOrFinish(Certificate::ref certificate, boost::shared_ptr<CertificateVerificationError> error);

		private:
			JID localJID;
			State state;
			boost::shared_ptr<SessionStream> stream;
			bool allowPLAINOverNonTLS;
			bool useStreamCompression;
			UseTLS useTLS;
			bool useAcks;
			bool needSessionStart;
			bool needResourceBind;
			bool needAcking;
			bool rosterVersioningSupported;
			ClientAuthenticator* authenticator;
			boost::shared_ptr<StanzaAckRequester> stanzaAckRequester_;
			boost::shared_ptr<StanzaAckResponder> stanzaAckResponder_;
			boost::shared_ptr<Swift::Error> error_;
			CertificateTrustChecker* certificateTrustChecker;
	};
}
