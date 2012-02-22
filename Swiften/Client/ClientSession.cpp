/*
 * Copyright (c) 2010 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#include <Swiften/Client/ClientSession.h>

#include <boost/bind.hpp>
#include <boost/uuid/uuid.hpp>
#include <boost/uuid/uuid_io.hpp>
#include <boost/uuid/uuid_generators.hpp>
#include <boost/smart_ptr/make_shared.hpp>

#include <Swiften/Base/Platform.h>
#include <Swiften/Base/Log.h>
#include <Swiften/Elements/ProtocolHeader.h>
#include <Swiften/Elements/StreamFeatures.h>
#include <Swiften/Elements/StreamError.h>
#include <Swiften/Elements/StartTLSRequest.h>
#include <Swiften/Elements/StartTLSFailure.h>
#include <Swiften/Elements/TLSProceed.h>
#include <Swiften/Elements/AuthRequest.h>
#include <Swiften/Elements/AuthSuccess.h>
#include <Swiften/Elements/AuthFailure.h>
#include <Swiften/Elements/AuthChallenge.h>
#include <Swiften/Elements/AuthResponse.h>
#include <Swiften/Elements/Compressed.h>
#include <Swiften/Elements/CompressFailure.h>
#include <Swiften/Elements/CompressRequest.h>
#include <Swiften/Elements/EnableStreamManagement.h>
#include <Swiften/Elements/StreamManagementEnabled.h>
#include <Swiften/Elements/StreamManagementFailed.h>
#include <Swiften/Elements/StartSession.h>
#include <Swiften/Elements/StanzaAck.h>
#include <Swiften/Elements/StanzaAckRequest.h>
#include <Swiften/Elements/IQ.h>
#include <Swiften/Elements/ResourceBind.h>
#include <Swiften/SASL/PLAINClientAuthenticator.h>
#include <Swiften/SASL/SCRAMSHA1ClientAuthenticator.h>
#include <Swiften/SASL/DIGESTMD5ClientAuthenticator.h>
#include <Swiften/Session/SessionStream.h>
#include <Swiften/TLS/CertificateTrustChecker.h>
#include <Swiften/TLS/ServerIdentityVerifier.h>

#ifdef SWIFTEN_PLATFORM_WIN32
#include <Swiften/Base/WindowsRegistry.h>
#endif

namespace Swift {

ClientSession::ClientSession(
		const JID& jid, 
		boost::shared_ptr<SessionStream> stream) :
			localJID(jid),	
			state(Initial), 
			stream(stream),
			allowPLAINOverNonTLS(false),
			useStreamCompression(true),
			useTLS(UseTLSWhenAvailable),
			useAcks(true),
			needSessionStart(false),
			needResourceBind(false),
			needAcking(false),
			rosterVersioningSupported(false),
			authenticator(NULL),
			certificateTrustChecker(NULL) {
#ifdef SWIFTEN_PLATFORM_WIN32
if (WindowsRegistry::isFIPSEnabled()) {
	SWIFT_LOG("info") << "Windows is running in FIPS-140 mode. Some authentication methods will be unavailable." << std::endl;
}
#endif
}

ClientSession::~ClientSession() {
}

void ClientSession::start() {
	stream->onStreamStartReceived.connect(boost::bind(&ClientSession::handleStreamStart, shared_from_this(), _1));
	stream->onElementReceived.connect(boost::bind(&ClientSession::handleElement, shared_from_this(), _1));
	stream->onClosed.connect(boost::bind(&ClientSession::handleStreamClosed, shared_from_this(), _1));
	stream->onTLSEncrypted.connect(boost::bind(&ClientSession::handleTLSEncrypted, shared_from_this()));

	assert(state == Initial);
	state = WaitingForStreamStart;
	sendStreamHeader();
}

void ClientSession::sendStreamHeader() {
	ProtocolHeader header;
	header.setTo(getRemoteJID());
	stream->writeHeader(header);
}

void ClientSession::sendStanza(boost::shared_ptr<Stanza> stanza) {
	stream->writeElement(stanza);
	if (stanzaAckRequester_) {
		stanzaAckRequester_->handleStanzaSent(stanza);
	}
}

void ClientSession::handleStreamStart(const ProtocolHeader&) {
	checkState(WaitingForStreamStart);
	state = Negotiating;
}

void ClientSession::handleElement(boost::shared_ptr<Element> element) {
	if (boost::shared_ptr<Stanza> stanza = boost::dynamic_pointer_cast<Stanza>(element)) {
		if (stanzaAckResponder_) {
			stanzaAckResponder_->handleStanzaReceived();
		}
		if (getState() == Initialized) {
			onStanzaReceived(stanza);
		}
		else if (boost::shared_ptr<IQ> iq = boost::dynamic_pointer_cast<IQ>(element)) {
			if (state == BindingResource) {
				boost::shared_ptr<ResourceBind> resourceBind(iq->getPayload<ResourceBind>());
				if (iq->getType() == IQ::Error && iq->getID() == "session-bind") {
					finishSession(Error::ResourceBindError);
				}
				else if (!resourceBind) {
					finishSession(Error::UnexpectedElementError);
				}
				else if (iq->getType() == IQ::Result) {
					localJID = resourceBind->getJID();
					if (!localJID.isValid()) {
						finishSession(Error::ResourceBindError);
					}
					needResourceBind = false;
					continueSessionInitialization();
				}
				else {
					finishSession(Error::UnexpectedElementError);
				}
			}
			else if (state == StartingSession) {
				if (iq->getType() == IQ::Result) {
					needSessionStart = false;
					continueSessionInitialization();
				}
				else if (iq->getType() == IQ::Error) {
					finishSession(Error::SessionStartError);
				}
				else {
					finishSession(Error::UnexpectedElementError);
				}
			}
			else {
				finishSession(Error::UnexpectedElementError);
			}
		}
	}
	else if (boost::dynamic_pointer_cast<StanzaAckRequest>(element)) {
		if (stanzaAckResponder_) {
			stanzaAckResponder_->handleAckRequestReceived();
		}
	}
	else if (boost::shared_ptr<StanzaAck> ack = boost::dynamic_pointer_cast<StanzaAck>(element)) {
		if (stanzaAckRequester_) {
			if (ack->isValid()) {
				stanzaAckRequester_->handleAckReceived(ack->getHandledStanzasCount());
			}
			else {
				std::cerr << "Warning: Got invalid ack from server" << std::endl;
			}
		}
		else {
			std::cerr << "Warning: Ignoring ack" << std::endl;
		}
	}
	else if (StreamError::ref streamError = boost::dynamic_pointer_cast<StreamError>(element)) {
		finishSession(Error::StreamError);
	}
	else if (getState() == Initialized) {
		boost::shared_ptr<Stanza> stanza = boost::dynamic_pointer_cast<Stanza>(element);
		if (stanza) {
			if (stanzaAckResponder_) {
				stanzaAckResponder_->handleStanzaReceived();
			}
			onStanzaReceived(stanza);
		}
	}
	else if (StreamFeatures* streamFeatures = dynamic_cast<StreamFeatures*>(element.get())) {
		if (!checkState(Negotiating)) {
			return;
		}

		if (streamFeatures->hasStartTLS() && stream->supportsTLSEncryption() && useTLS != NeverUseTLS) {
			state = WaitingForEncrypt;
			stream->writeElement(boost::make_shared<StartTLSRequest>());
		}
		else if (useTLS == RequireTLS && !stream->isTLSEncrypted()) {
			finishSession(Error::NoSupportedAuthMechanismsError);
		}
		else if (useStreamCompression && stream->supportsZLibCompression() && streamFeatures->hasCompressionMethod("zlib")) {
			state = Compressing;
			stream->writeElement(boost::make_shared<CompressRequest>("zlib"));
		}
		else if (streamFeatures->hasAuthenticationMechanisms()) {
			if (stream->hasTLSCertificate()) {
				if (streamFeatures->hasAuthenticationMechanism("EXTERNAL")) {
					state = Authenticating;
					stream->writeElement(boost::make_shared<AuthRequest>("EXTERNAL", createSafeByteArray("")));
				}
				else {
					finishSession(Error::TLSClientCertificateError);
				}
			}
			else if (streamFeatures->hasAuthenticationMechanism("EXTERNAL")) {
				state = Authenticating;
				stream->writeElement(boost::make_shared<AuthRequest>("EXTERNAL", createSafeByteArray("")));
			}
			else if (streamFeatures->hasAuthenticationMechanism("SCRAM-SHA-1") || streamFeatures->hasAuthenticationMechanism("SCRAM-SHA-1-PLUS")) {
				std::ostringstream s;
				ByteArray finishMessage;
				bool plus = stream->isTLSEncrypted() && streamFeatures->hasAuthenticationMechanism("SCRAM-SHA-1-PLUS");
				if (plus) {
					finishMessage = stream->getTLSFinishMessage();
					plus &= !finishMessage.empty();
				}
				s << boost::uuids::random_generator()();
				SCRAMSHA1ClientAuthenticator* scramAuthenticator = new SCRAMSHA1ClientAuthenticator(s.str(), plus);
				if (plus) {
					scramAuthenticator->setTLSChannelBindingData(finishMessage);
				}
				authenticator = scramAuthenticator;
				state = WaitingForCredentials;
				onNeedCredentials();
			}
			else if ((stream->isTLSEncrypted() || allowPLAINOverNonTLS) && streamFeatures->hasAuthenticationMechanism("PLAIN")) {
				authenticator = new PLAINClientAuthenticator();
				state = WaitingForCredentials;
				onNeedCredentials();
			}
			else if (streamFeatures->hasAuthenticationMechanism("DIGEST-MD5") && DIGESTMD5ClientAuthenticator::canBeUsed()) {
				std::ostringstream s;
				s << boost::uuids::random_generator()();
				// FIXME: Host should probably be the actual host
				authenticator = new DIGESTMD5ClientAuthenticator(localJID.getDomain(), s.str());
				state = WaitingForCredentials;
				onNeedCredentials();
			}
			else {
				finishSession(Error::NoSupportedAuthMechanismsError);
			}
		}
		else {
			// Start the session
			rosterVersioningSupported = streamFeatures->hasRosterVersioning();
			stream->setWhitespacePingEnabled(true);
			needSessionStart = streamFeatures->hasSession();
			needResourceBind = streamFeatures->hasResourceBind();
			needAcking = streamFeatures->hasStreamManagement() && useAcks;
			if (!needResourceBind) {
				// Resource binding is a MUST
				finishSession(Error::ResourceBindError);
			}
			else {
				continueSessionInitialization();
			}
		}
	}
	else if (boost::dynamic_pointer_cast<Compressed>(element)) {
		checkState(Compressing);
		state = WaitingForStreamStart;
		stream->addZLibCompression();
		stream->resetXMPPParser();
		sendStreamHeader();
	}
	else if (boost::dynamic_pointer_cast<CompressFailure>(element)) {
		finishSession(Error::CompressionFailedError);
	}
	else if (boost::dynamic_pointer_cast<StreamManagementEnabled>(element)) {
		stanzaAckRequester_ = boost::make_shared<StanzaAckRequester>();
		stanzaAckRequester_->onRequestAck.connect(boost::bind(&ClientSession::requestAck, shared_from_this()));
		stanzaAckRequester_->onStanzaAcked.connect(boost::bind(&ClientSession::handleStanzaAcked, shared_from_this(), _1));
		stanzaAckResponder_ = boost::make_shared<StanzaAckResponder>();
		stanzaAckResponder_->onAck.connect(boost::bind(&ClientSession::ack, shared_from_this(), _1));
		needAcking = false;
		continueSessionInitialization();
	}
	else if (boost::dynamic_pointer_cast<StreamManagementFailed>(element)) {
		needAcking = false;
		continueSessionInitialization();
	}
	else if (AuthChallenge* challenge = dynamic_cast<AuthChallenge*>(element.get())) {
		checkState(Authenticating);
		assert(authenticator);
		if (authenticator->setChallenge(challenge->getValue())) {
			stream->writeElement(boost::make_shared<AuthResponse>(authenticator->getResponse()));
		}
		else {
			finishSession(Error::AuthenticationFailedError);
		}
	}
	else if (AuthSuccess* authSuccess = dynamic_cast<AuthSuccess*>(element.get())) {
		checkState(Authenticating);
		if (authenticator && !authenticator->setChallenge(authSuccess->getValue())) {
			delete authenticator;
			authenticator = NULL;
			finishSession(Error::ServerVerificationFailedError);
		}
		else {
			state = WaitingForStreamStart;
			delete authenticator;
			authenticator = NULL;
			stream->resetXMPPParser();
			sendStreamHeader();
		}
	}
	else if (dynamic_cast<AuthFailure*>(element.get())) {
		delete authenticator;
		authenticator = NULL;
		finishSession(Error::AuthenticationFailedError);
	}
	else if (dynamic_cast<TLSProceed*>(element.get())) {
		checkState(WaitingForEncrypt);
		state = Encrypting;
		stream->addTLSEncryption();
	}
	else if (dynamic_cast<StartTLSFailure*>(element.get())) {
		finishSession(Error::TLSError);
	}
	else {
		// FIXME Not correct?
		state = Initialized;
		onInitialized();
	}
}

void ClientSession::continueSessionInitialization() {
	if (needResourceBind) {
		state = BindingResource;
		boost::shared_ptr<ResourceBind> resourceBind(boost::make_shared<ResourceBind>());
		if (!localJID.getResource().empty()) {
			resourceBind->setResource(localJID.getResource());
		}
		sendStanza(IQ::createRequest(IQ::Set, JID(), "session-bind", resourceBind));
	}
	else if (needAcking) {
		state = EnablingSessionManagement;
		stream->writeElement(boost::make_shared<EnableStreamManagement>());
	}
	else if (needSessionStart) {
		state = StartingSession;
		sendStanza(IQ::createRequest(IQ::Set, JID(), "session-start", boost::make_shared<StartSession>()));
	}
	else {
		state = Initialized;
		onInitialized();
	}
}

bool ClientSession::checkState(State state) {
	if (this->state != state) {
		finishSession(Error::UnexpectedElementError);
		return false;
	}
	return true;
}

void ClientSession::sendCredentials(const SafeByteArray& password) {
	assert(WaitingForCredentials);
	state = Authenticating;
	authenticator->setCredentials(localJID.getNode(), password);
	stream->writeElement(boost::make_shared<AuthRequest>(authenticator->getName(), authenticator->getResponse()));
}

void ClientSession::handleTLSEncrypted() {
	checkState(Encrypting);

	Certificate::ref certificate = stream->getPeerCertificate();
	boost::shared_ptr<CertificateVerificationError> verificationError = stream->getPeerCertificateVerificationError();
	if (verificationError) {
		checkTrustOrFinish(certificate, verificationError);
	}
	else {
		ServerIdentityVerifier identityVerifier(localJID);
		if (identityVerifier.certificateVerifies(certificate)) {
			continueAfterTLSEncrypted();
		}
		else {
			checkTrustOrFinish(certificate, boost::make_shared<CertificateVerificationError>(CertificateVerificationError::InvalidServerIdentity));
		}
	}
}

void ClientSession::checkTrustOrFinish(Certificate::ref certificate, boost::shared_ptr<CertificateVerificationError> error) {
	if (certificateTrustChecker && certificateTrustChecker->isCertificateTrusted(certificate)) {
		continueAfterTLSEncrypted();
	}
	else {
		finishSession(error);
	}
}

void ClientSession::continueAfterTLSEncrypted() {
	state = WaitingForStreamStart;
	stream->resetXMPPParser();
	sendStreamHeader();
}

void ClientSession::handleStreamClosed(boost::shared_ptr<Swift::Error> streamError) {
	State previousState = state;
	state = Finished;

	if (stanzaAckRequester_) {
		stanzaAckRequester_->onRequestAck.disconnect(boost::bind(&ClientSession::requestAck, shared_from_this()));
		stanzaAckRequester_->onStanzaAcked.disconnect(boost::bind(&ClientSession::handleStanzaAcked, shared_from_this(), _1));
		stanzaAckRequester_.reset();
	}
	if (stanzaAckResponder_) {
		stanzaAckResponder_->onAck.disconnect(boost::bind(&ClientSession::ack, shared_from_this(), _1));
		stanzaAckResponder_.reset();
	}
	stream->setWhitespacePingEnabled(false);
	stream->onStreamStartReceived.disconnect(boost::bind(&ClientSession::handleStreamStart, shared_from_this(), _1));
	stream->onElementReceived.disconnect(boost::bind(&ClientSession::handleElement, shared_from_this(), _1));
	stream->onClosed.disconnect(boost::bind(&ClientSession::handleStreamClosed, shared_from_this(), _1));
	stream->onTLSEncrypted.disconnect(boost::bind(&ClientSession::handleTLSEncrypted, shared_from_this()));

	if (previousState == Finishing) {
		onFinished(error_);
	}
	else {
		onFinished(streamError);
	}
}

void ClientSession::finish() {
	finishSession(boost::shared_ptr<Error>());
}

void ClientSession::finishSession(Error::Type error) {
	finishSession(boost::make_shared<Swift::ClientSession::Error>(error));
}

void ClientSession::finishSession(boost::shared_ptr<Swift::Error> error) {
	state = Finishing;
	if (!error_) {
		error_ = error;
	}
	assert(stream->isOpen());
	if (stanzaAckResponder_) {
		stanzaAckResponder_->handleAckRequestReceived();
	}
	stream->writeFooter();
	stream->close();
}

void ClientSession::requestAck() {
	stream->writeElement(boost::make_shared<StanzaAckRequest>());
}

void ClientSession::handleStanzaAcked(boost::shared_ptr<Stanza> stanza) {
	onStanzaAcked(stanza);
}

void ClientSession::ack(unsigned int handledStanzasCount) {
	stream->writeElement(boost::make_shared<StanzaAck>(handledStanzasCount));
}

}
