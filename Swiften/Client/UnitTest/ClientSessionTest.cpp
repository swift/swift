/*
 * Copyright (c) 2010 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/extensions/TestFactoryRegistry.h>
#include <deque>
#include <boost/bind.hpp>
#include <boost/optional.hpp>
#include <boost/smart_ptr/make_shared.hpp>

#include <Swiften/Session/SessionStream.h>
#include <Swiften/Client/ClientSession.h>
#include <Swiften/Elements/Message.h>
#include <Swiften/Elements/StartTLSRequest.h>
#include <Swiften/Elements/StreamFeatures.h>
#include <Swiften/Elements/StreamError.h>
#include <Swiften/Elements/TLSProceed.h>
#include <Swiften/Elements/StartTLSFailure.h>
#include <Swiften/Elements/AuthRequest.h>
#include <Swiften/Elements/AuthSuccess.h>
#include <Swiften/Elements/AuthFailure.h>
#include <Swiften/Elements/StreamManagementEnabled.h>
#include <Swiften/Elements/StreamManagementFailed.h>
#include <Swiften/Elements/StanzaAck.h>
#include <Swiften/Elements/EnableStreamManagement.h>
#include <Swiften/Elements/IQ.h>
#include <Swiften/Elements/ResourceBind.h>
#include <Swiften/TLS/SimpleCertificate.h>
#include <Swiften/TLS/BlindCertificateTrustChecker.h>

using namespace Swift;

class ClientSessionTest : public CppUnit::TestFixture {
		CPPUNIT_TEST_SUITE(ClientSessionTest);
		CPPUNIT_TEST(testStart_Error);
		CPPUNIT_TEST(testStart_StreamError);
		CPPUNIT_TEST(testStartTLS);
		CPPUNIT_TEST(testStartTLS_ServerError);
		CPPUNIT_TEST(testStartTLS_ConnectError);
		CPPUNIT_TEST(testStartTLS_InvalidIdentity);
		CPPUNIT_TEST(testStart_StreamFeaturesWithoutResourceBindingFails);
		CPPUNIT_TEST(testAuthenticate);
		CPPUNIT_TEST(testAuthenticate_Unauthorized);
		CPPUNIT_TEST(testAuthenticate_NoValidAuthMechanisms);
		CPPUNIT_TEST(testAuthenticate_PLAINOverNonTLS);
		CPPUNIT_TEST(testAuthenticate_RequireTLS);
		CPPUNIT_TEST(testStreamManagement);
		CPPUNIT_TEST(testStreamManagement_Failed);
		CPPUNIT_TEST(testFinishAcksStanzas);
		/*
		CPPUNIT_TEST(testResourceBind);
		CPPUNIT_TEST(testResourceBind_ChangeResource);
		CPPUNIT_TEST(testResourceBind_EmptyResource);
		CPPUNIT_TEST(testResourceBind_Error);
		CPPUNIT_TEST(testSessionStart);
		CPPUNIT_TEST(testSessionStart_Error);
		CPPUNIT_TEST(testSessionStart_AfterResourceBind);
		CPPUNIT_TEST(testWhitespacePing);
		CPPUNIT_TEST(testReceiveElementAfterSessionStarted);
		CPPUNIT_TEST(testSendElement);
		*/
		CPPUNIT_TEST_SUITE_END();

	public:
		void setUp() {
			server = boost::make_shared<MockSessionStream>();
			sessionFinishedReceived = false;
			needCredentials = false;
			blindCertificateTrustChecker = new BlindCertificateTrustChecker();
		}

		void tearDown() {
			delete blindCertificateTrustChecker;
		}

		void testStart_Error() {
			boost::shared_ptr<ClientSession> session(createSession());
			session->start();
			server->breakConnection();

			CPPUNIT_ASSERT_EQUAL(ClientSession::Finished, session->getState());
			CPPUNIT_ASSERT(sessionFinishedReceived);
			CPPUNIT_ASSERT(sessionFinishedError);
		}

		void testStart_StreamError() {
			boost::shared_ptr<ClientSession> session(createSession());
			session->start();
			server->sendStreamStart();
			server->sendStreamError();

			CPPUNIT_ASSERT_EQUAL(ClientSession::Finished, session->getState());
			CPPUNIT_ASSERT(sessionFinishedReceived);
			CPPUNIT_ASSERT(sessionFinishedError);
		}

		void testStartTLS() {
			boost::shared_ptr<ClientSession> session(createSession());
			session->setCertificateTrustChecker(blindCertificateTrustChecker);
			session->start();
			server->receiveStreamStart();
			server->sendStreamStart();
			server->sendStreamFeaturesWithStartTLS();
			server->receiveStartTLS();
			CPPUNIT_ASSERT(!server->tlsEncrypted);
			server->sendTLSProceed();
			CPPUNIT_ASSERT(server->tlsEncrypted);
			server->onTLSEncrypted();
			server->receiveStreamStart();
			server->sendStreamStart();

			session->finish();
		}

		void testStartTLS_ServerError() {
			boost::shared_ptr<ClientSession> session(createSession());
			session->start();
			server->receiveStreamStart();
			server->sendStreamStart();
			server->sendStreamFeaturesWithStartTLS();
			server->receiveStartTLS();
			server->sendTLSFailure();

			CPPUNIT_ASSERT(!server->tlsEncrypted);
			CPPUNIT_ASSERT_EQUAL(ClientSession::Finished, session->getState());
			CPPUNIT_ASSERT(sessionFinishedReceived);
			CPPUNIT_ASSERT(sessionFinishedError);
		}

		void testStartTLS_ConnectError() {
			boost::shared_ptr<ClientSession> session(createSession());
			session->start();
			server->receiveStreamStart();
			server->sendStreamStart();
			server->sendStreamFeaturesWithStartTLS();
			server->receiveStartTLS();
			server->sendTLSProceed();
			server->breakTLS();

			CPPUNIT_ASSERT_EQUAL(ClientSession::Finished, session->getState());
			CPPUNIT_ASSERT(sessionFinishedReceived);
			CPPUNIT_ASSERT(sessionFinishedError);
		}

		void testStartTLS_InvalidIdentity() {
			boost::shared_ptr<ClientSession> session(createSession());
			session->start();
			server->receiveStreamStart();
			server->sendStreamStart();
			server->sendStreamFeaturesWithStartTLS();
			server->receiveStartTLS();
			CPPUNIT_ASSERT(!server->tlsEncrypted);
			server->sendTLSProceed();
			CPPUNIT_ASSERT(server->tlsEncrypted);
			server->onTLSEncrypted();

			CPPUNIT_ASSERT_EQUAL(ClientSession::Finished, session->getState());
			CPPUNIT_ASSERT(sessionFinishedReceived);
			CPPUNIT_ASSERT(sessionFinishedError);
			CPPUNIT_ASSERT_EQUAL(CertificateVerificationError::InvalidServerIdentity, boost::dynamic_pointer_cast<CertificateVerificationError>(sessionFinishedError)->getType());
		}

		void testStart_StreamFeaturesWithoutResourceBindingFails() {
			boost::shared_ptr<ClientSession> session(createSession());
			session->start();
			server->receiveStreamStart();
			server->sendStreamStart();
			server->sendEmptyStreamFeatures();

			CPPUNIT_ASSERT_EQUAL(ClientSession::Finished, session->getState());
			CPPUNIT_ASSERT(sessionFinishedReceived);
			CPPUNIT_ASSERT(sessionFinishedError);
		}

		void testAuthenticate() {
			boost::shared_ptr<ClientSession> session(createSession());
			session->start();
			server->receiveStreamStart();
			server->sendStreamStart();
			server->sendStreamFeaturesWithPLAINAuthentication();
			CPPUNIT_ASSERT(needCredentials);
			CPPUNIT_ASSERT_EQUAL(ClientSession::WaitingForCredentials, session->getState());
			session->sendCredentials(createSafeByteArray("mypass"));
			server->receiveAuthRequest("PLAIN");
			server->sendAuthSuccess();
			server->receiveStreamStart();

			session->finish();
		}

		void testAuthenticate_Unauthorized() {
			boost::shared_ptr<ClientSession> session(createSession());
			session->start();
			server->receiveStreamStart();
			server->sendStreamStart();
			server->sendStreamFeaturesWithPLAINAuthentication();
			CPPUNIT_ASSERT(needCredentials);
			CPPUNIT_ASSERT_EQUAL(ClientSession::WaitingForCredentials, session->getState());
			session->sendCredentials(createSafeByteArray("mypass"));
			server->receiveAuthRequest("PLAIN");
			server->sendAuthFailure();

			CPPUNIT_ASSERT_EQUAL(ClientSession::Finished, session->getState());
			CPPUNIT_ASSERT(sessionFinishedReceived);
			CPPUNIT_ASSERT(sessionFinishedError);
		}

		void testAuthenticate_PLAINOverNonTLS() {
			boost::shared_ptr<ClientSession> session(createSession());
			session->setAllowPLAINOverNonTLS(false);
			session->start();
			server->receiveStreamStart();
			server->sendStreamStart();
			server->sendStreamFeaturesWithPLAINAuthentication();

			CPPUNIT_ASSERT_EQUAL(ClientSession::Finished, session->getState());
			CPPUNIT_ASSERT(sessionFinishedReceived);
			CPPUNIT_ASSERT(sessionFinishedError);
		}

		void testAuthenticate_RequireTLS() {
			boost::shared_ptr<ClientSession> session(createSession());
			session->setUseTLS(ClientSession::RequireTLS);
			session->setAllowPLAINOverNonTLS(true);
			session->start();
			server->receiveStreamStart();
			server->sendStreamStart();
			server->sendStreamFeaturesWithMultipleAuthentication();

			CPPUNIT_ASSERT_EQUAL(ClientSession::Finished, session->getState());
			CPPUNIT_ASSERT(sessionFinishedReceived);
			CPPUNIT_ASSERT(sessionFinishedError);
		}

		void testAuthenticate_NoValidAuthMechanisms() {
			boost::shared_ptr<ClientSession> session(createSession());
			session->start();
			server->receiveStreamStart();
			server->sendStreamStart();
			server->sendStreamFeaturesWithUnknownAuthentication();

			CPPUNIT_ASSERT_EQUAL(ClientSession::Finished, session->getState());
			CPPUNIT_ASSERT(sessionFinishedReceived);
			CPPUNIT_ASSERT(sessionFinishedError);
		}

		void testStreamManagement() {
			boost::shared_ptr<ClientSession> session(createSession());
			session->start();
			server->receiveStreamStart();
			server->sendStreamStart();
			server->sendStreamFeaturesWithPLAINAuthentication();
			session->sendCredentials(createSafeByteArray("mypass"));
			server->receiveAuthRequest("PLAIN");
			server->sendAuthSuccess();
			server->receiveStreamStart();
			server->sendStreamStart();
			server->sendStreamFeaturesWithBindAndStreamManagement();
			server->receiveBind();
			server->sendBindResult();
			server->receiveStreamManagementEnable();
			server->sendStreamManagementEnabled();

			CPPUNIT_ASSERT(session->getStreamManagementEnabled());
			// TODO: Test if the requesters & responders do their work
			CPPUNIT_ASSERT_EQUAL(ClientSession::Initialized, session->getState());

			session->finish();
		}

		void testStreamManagement_Failed() {
			boost::shared_ptr<ClientSession> session(createSession());
			session->start();
			server->receiveStreamStart();
			server->sendStreamStart();
			server->sendStreamFeaturesWithPLAINAuthentication();
			session->sendCredentials(createSafeByteArray("mypass"));
			server->receiveAuthRequest("PLAIN");
			server->sendAuthSuccess();
			server->receiveStreamStart();
			server->sendStreamStart();
			server->sendStreamFeaturesWithBindAndStreamManagement();
			server->receiveBind();
			server->sendBindResult();
			server->receiveStreamManagementEnable();
			server->sendStreamManagementFailed();

			CPPUNIT_ASSERT(!session->getStreamManagementEnabled());
			CPPUNIT_ASSERT_EQUAL(ClientSession::Initialized, session->getState());

			session->finish();
		}

		void testFinishAcksStanzas() {
			boost::shared_ptr<ClientSession> session(createSession());
			initializeSession(session);
			server->sendMessage();
			server->sendMessage();
			server->sendMessage();

			session->finish();

			server->receiveAck(3);
		}

	private:
		boost::shared_ptr<ClientSession> createSession() {
			boost::shared_ptr<ClientSession> session = ClientSession::create(JID("me@foo.com"), server);
			session->onFinished.connect(boost::bind(&ClientSessionTest::handleSessionFinished, this, _1));
			session->onNeedCredentials.connect(boost::bind(&ClientSessionTest::handleSessionNeedCredentials, this));
			session->setAllowPLAINOverNonTLS(true);
			return session;
		}

		void initializeSession(boost::shared_ptr<ClientSession> session) {
			session->start();
			server->receiveStreamStart();
			server->sendStreamStart();
			server->sendStreamFeaturesWithPLAINAuthentication();
			session->sendCredentials(createSafeByteArray("mypass"));
			server->receiveAuthRequest("PLAIN");
			server->sendAuthSuccess();
			server->receiveStreamStart();
			server->sendStreamStart();
			server->sendStreamFeaturesWithBindAndStreamManagement();
			server->receiveBind();
			server->sendBindResult();
			server->receiveStreamManagementEnable();
			server->sendStreamManagementEnabled();
		}

		void handleSessionFinished(boost::shared_ptr<Error> error) {
			sessionFinishedReceived = true;
			sessionFinishedError = error;
		}

		void handleSessionNeedCredentials() {
			needCredentials = true;
		}

		class MockSessionStream : public SessionStream {
			public:
				struct Event {
					Event(boost::shared_ptr<Element> element) : element(element), footer(false) {}
					Event(const ProtocolHeader& header) : header(header), footer(false) {}
					Event() : footer(true) {}
					
					boost::shared_ptr<Element> element;
					boost::optional<ProtocolHeader> header;
					bool footer;
				};

				MockSessionStream() : available(true), canTLSEncrypt(true), tlsEncrypted(false), compressed(false), whitespacePingEnabled(false), resetCount(0) {
				}

				virtual void close() {
					onClosed(boost::shared_ptr<Error>());
				}

				virtual bool isOpen() {
					return available;
				}

				virtual void writeHeader(const ProtocolHeader& header) {
					receivedEvents.push_back(Event(header));
				}

				virtual void writeFooter() {
					receivedEvents.push_back(Event());
				}

				virtual void writeElement(boost::shared_ptr<Element> element) {
					receivedEvents.push_back(Event(element));
				}

				virtual void writeData(const std::string&) {
				}

				virtual bool supportsTLSEncryption() {
					return canTLSEncrypt;
				}

				virtual void addTLSEncryption() {
					tlsEncrypted = true;
				}

				virtual bool isTLSEncrypted() {
					return tlsEncrypted;
				}

				virtual ByteArray getTLSFinishMessage() const {
					return ByteArray();
				}

				virtual Certificate::ref getPeerCertificate() const {
					return Certificate::ref(new SimpleCertificate());
				}

				virtual boost::shared_ptr<CertificateVerificationError> getPeerCertificateVerificationError() const {
					return boost::shared_ptr<CertificateVerificationError>();
				}

				virtual bool supportsZLibCompression() {
					return true;
				}
			
				virtual void addZLibCompression() {
					compressed = true;
				}

				virtual void setWhitespacePingEnabled(bool enabled) {
					whitespacePingEnabled = enabled;
				}

				virtual void resetXMPPParser() {
					resetCount++;
				}

				void breakConnection() {
					onClosed(boost::make_shared<SessionStream::SessionStreamError>(SessionStream::SessionStreamError::ConnectionReadError));
				}

				void breakTLS() {
					onClosed(boost::make_shared<SessionStream::SessionStreamError>(SessionStream::SessionStreamError::TLSError));
				}


				void sendStreamStart() {
					ProtocolHeader header;
					header.setTo("foo.com");
					return onStreamStartReceived(header);
				}

				void sendStreamFeaturesWithStartTLS() {
					boost::shared_ptr<StreamFeatures> streamFeatures(new StreamFeatures());
					streamFeatures->setHasStartTLS();
					onElementReceived(streamFeatures);
				}

				void sendStreamError() {
					onElementReceived(boost::make_shared<StreamError>());
				}

				void sendTLSProceed() {
					onElementReceived(boost::make_shared<TLSProceed>());
				}

				void sendTLSFailure() {
					onElementReceived(boost::make_shared<StartTLSFailure>());
				}

				void sendStreamFeaturesWithMultipleAuthentication() {
					boost::shared_ptr<StreamFeatures> streamFeatures(new StreamFeatures());
					streamFeatures->addAuthenticationMechanism("PLAIN");
					streamFeatures->addAuthenticationMechanism("DIGEST-MD5");
					streamFeatures->addAuthenticationMechanism("SCRAM-SHA1");
					onElementReceived(streamFeatures);
				}

				void sendStreamFeaturesWithPLAINAuthentication() {
					boost::shared_ptr<StreamFeatures> streamFeatures(new StreamFeatures());
					streamFeatures->addAuthenticationMechanism("PLAIN");
					onElementReceived(streamFeatures);
				}

				void sendStreamFeaturesWithUnknownAuthentication() {
					boost::shared_ptr<StreamFeatures> streamFeatures(new StreamFeatures());
					streamFeatures->addAuthenticationMechanism("UNKNOWN");
					onElementReceived(streamFeatures);
				}

				void sendStreamFeaturesWithBindAndStreamManagement() {
					boost::shared_ptr<StreamFeatures> streamFeatures(new StreamFeatures());
					streamFeatures->setHasResourceBind();
					streamFeatures->setHasStreamManagement();
					onElementReceived(streamFeatures);
				}

				void sendEmptyStreamFeatures() {
					onElementReceived(boost::make_shared<StreamFeatures>());
				}

				void sendAuthSuccess() {
					onElementReceived(boost::make_shared<AuthSuccess>());
				}

				void sendAuthFailure() {
					onElementReceived(boost::make_shared<AuthFailure>());
				}

				void sendStreamManagementEnabled() {
					onElementReceived(boost::make_shared<StreamManagementEnabled>());
				}

				void sendStreamManagementFailed() {
					onElementReceived(boost::make_shared<StreamManagementFailed>());
				}

				void sendBindResult() {
					boost::shared_ptr<ResourceBind> resourceBind(new ResourceBind());
					resourceBind->setJID(JID("foo@bar.com/bla"));
					boost::shared_ptr<IQ> iq = IQ::createResult(JID("foo@bar.com"), bindID, resourceBind);
					onElementReceived(iq);
				}

				void sendMessage() {
					boost::shared_ptr<Message> message = boost::make_shared<Message>();
					message->setTo(JID("foo@bar.com/bla"));
					onElementReceived(message);
				}

				void receiveStreamStart() {
					Event event = popEvent();
					CPPUNIT_ASSERT(event.header);
				}

				void receiveStartTLS() {
					Event event = popEvent();
					CPPUNIT_ASSERT(event.element);
					CPPUNIT_ASSERT(boost::dynamic_pointer_cast<StartTLSRequest>(event.element));
				}

				void receiveAuthRequest(const std::string& mech) {
					Event event = popEvent();
					CPPUNIT_ASSERT(event.element);
					boost::shared_ptr<AuthRequest> request(boost::dynamic_pointer_cast<AuthRequest>(event.element));
					CPPUNIT_ASSERT(request);
					CPPUNIT_ASSERT_EQUAL(mech, request->getMechanism());
				}

				void receiveStreamManagementEnable() {
					Event event = popEvent();
					CPPUNIT_ASSERT(event.element);
					CPPUNIT_ASSERT(boost::dynamic_pointer_cast<EnableStreamManagement>(event.element));
				}

				void receiveBind() {
					Event event = popEvent();
					CPPUNIT_ASSERT(event.element);
					boost::shared_ptr<IQ> iq = boost::dynamic_pointer_cast<IQ>(event.element);
					CPPUNIT_ASSERT(iq);
					CPPUNIT_ASSERT(iq->getPayload<ResourceBind>());
					bindID = iq->getID();
				}

				void receiveAck(unsigned int n) {
					Event event = popEvent();
					CPPUNIT_ASSERT(event.element);
					boost::shared_ptr<StanzaAck> ack = boost::dynamic_pointer_cast<StanzaAck>(event.element);
					CPPUNIT_ASSERT(ack);
					CPPUNIT_ASSERT_EQUAL(n, ack->getHandledStanzasCount());
				}

				Event popEvent() {
					CPPUNIT_ASSERT(!receivedEvents.empty());
					Event event = receivedEvents.front();
					receivedEvents.pop_front();
					return event;
				}

				bool available;
				bool canTLSEncrypt;
				bool tlsEncrypted;
				bool compressed;
				bool whitespacePingEnabled;
				std::string bindID;
				int resetCount;
				std::deque<Event> receivedEvents;
		};

		boost::shared_ptr<MockSessionStream> server;
		bool sessionFinishedReceived;
		bool needCredentials;
		boost::shared_ptr<Error> sessionFinishedError;
		BlindCertificateTrustChecker* blindCertificateTrustChecker;
};

CPPUNIT_TEST_SUITE_REGISTRATION(ClientSessionTest);

#if 0
		void testAuthenticate() {
			boost::shared_ptr<MockSession> session(createSession("me@foo.com/Bar"));
			session->onNeedCredentials.connect(boost::bind(&ClientSessionTest::setNeedCredentials, this));
			getMockServer()->expectStreamStart();
			getMockServer()->sendStreamStart();
			getMockServer()->sendStreamFeaturesWithAuthentication();
			session->startSession();
			processEvents();
			CPPUNIT_ASSERT_EQUAL(ClientSession::WaitingForCredentials, session->getState());
			CPPUNIT_ASSERT(needCredentials_);

			getMockServer()->expectAuth("me", "mypass");
			getMockServer()->sendAuthSuccess();
			getMockServer()->expectStreamStart();
			getMockServer()->sendStreamStart();
			session->sendCredentials("mypass");
			CPPUNIT_ASSERT_EQUAL(ClientSession::Authenticating, session->getState());
			processEvents();
			CPPUNIT_ASSERT_EQUAL(ClientSession::Negotiating, session->getState());
		}

		void testAuthenticate_Unauthorized() {
			boost::shared_ptr<MockSession> session(createSession("me@foo.com/Bar"));
			getMockServer()->expectStreamStart();
			getMockServer()->sendStreamStart();
			getMockServer()->sendStreamFeaturesWithAuthentication();
			session->startSession();
			processEvents();

			getMockServer()->expectAuth("me", "mypass");
			getMockServer()->sendAuthFailure();
			session->sendCredentials("mypass");
			processEvents();

			CPPUNIT_ASSERT_EQUAL(ClientSession::Error, session->getState());
			CPPUNIT_ASSERT_EQUAL(ClientSession::AuthenticationFailedError, *session->getError());
		}

		void testAuthenticate_NoValidAuthMechanisms() {
			boost::shared_ptr<MockSession> session(createSession("me@foo.com/Bar"));
			getMockServer()->expectStreamStart();
			getMockServer()->sendStreamStart();
			getMockServer()->sendStreamFeaturesWithUnsupportedAuthentication();
			session->startSession();
			processEvents();

			CPPUNIT_ASSERT_EQUAL(ClientSession::Error, session->getState());
			CPPUNIT_ASSERT_EQUAL(ClientSession::NoSupportedAuthMechanismsError, *session->getError());
		}

		void testResourceBind() {
			boost::shared_ptr<MockSession> session(createSession("me@foo.com/Bar"));
			getMockServer()->expectStreamStart();
			getMockServer()->sendStreamStart();
			getMockServer()->sendStreamFeaturesWithResourceBind();
			getMockServer()->expectResourceBind("Bar", "session-bind");
			// FIXME: Check CPPUNIT_ASSERT_EQUAL(ClientSession::BindingResource, session->getState());
			getMockServer()->sendResourceBindResponse("me@foo.com/Bar", "session-bind");
			session->startSession();

			processEvents();

			CPPUNIT_ASSERT_EQUAL(ClientSession::SessionStarted, session->getState());
			CPPUNIT_ASSERT_EQUAL(JID("me@foo.com/Bar"), session->getLocalJID());
		}

		void testResourceBind_ChangeResource() {
			boost::shared_ptr<MockSession> session(createSession("me@foo.com/Bar"));
			getMockServer()->expectStreamStart();
			getMockServer()->sendStreamStart();
			getMockServer()->sendStreamFeaturesWithResourceBind();
			getMockServer()->expectResourceBind("Bar", "session-bind");
			getMockServer()->sendResourceBindResponse("me@foo.com/Bar123", "session-bind");
			session->startSession();
			processEvents();

			CPPUNIT_ASSERT_EQUAL(ClientSession::SessionStarted, session->getState());
			CPPUNIT_ASSERT_EQUAL(JID("me@foo.com/Bar123"), session->getLocalJID());
		}

		void testResourceBind_EmptyResource() {
			boost::shared_ptr<MockSession> session(createSession("me@foo.com"));
			getMockServer()->expectStreamStart();
			getMockServer()->sendStreamStart();
			getMockServer()->sendStreamFeaturesWithResourceBind();
			getMockServer()->expectResourceBind("", "session-bind");
			getMockServer()->sendResourceBindResponse("me@foo.com/NewResource", "session-bind");
			session->startSession();
			processEvents();

			CPPUNIT_ASSERT_EQUAL(ClientSession::SessionStarted, session->getState());
			CPPUNIT_ASSERT_EQUAL(JID("me@foo.com/NewResource"), session->getLocalJID());
		}

		void testResourceBind_Error() {
			boost::shared_ptr<MockSession> session(createSession("me@foo.com"));
			getMockServer()->expectStreamStart();
			getMockServer()->sendStreamStart();
			getMockServer()->sendStreamFeaturesWithResourceBind();
			getMockServer()->expectResourceBind("", "session-bind");
			getMockServer()->sendError("session-bind");
			session->startSession();
			processEvents();

			CPPUNIT_ASSERT_EQUAL(ClientSession::Error, session->getState());
			CPPUNIT_ASSERT_EQUAL(ClientSession::ResourceBindError, *session->getError());
		}

		void testSessionStart() {
			boost::shared_ptr<MockSession> session(createSession("me@foo.com/Bar"));
			session->onSessionStarted.connect(boost::bind(&ClientSessionTest::setSessionStarted, this));
			getMockServer()->expectStreamStart();
			getMockServer()->sendStreamStart();
			getMockServer()->sendStreamFeaturesWithSession();
			getMockServer()->expectSessionStart("session-start");
			// FIXME: Check CPPUNIT_ASSERT_EQUAL(ClientSession::StartingSession, session->getState());
			getMockServer()->sendSessionStartResponse("session-start");
			session->startSession();
			processEvents();

			CPPUNIT_ASSERT_EQUAL(ClientSession::SessionStarted, session->getState());
			CPPUNIT_ASSERT(sessionStarted_);
		}

		void testSessionStart_Error() {
			boost::shared_ptr<MockSession> session(createSession("me@foo.com/Bar"));
			getMockServer()->expectStreamStart();
			getMockServer()->sendStreamStart();
			getMockServer()->sendStreamFeaturesWithSession();
			getMockServer()->expectSessionStart("session-start");
			getMockServer()->sendError("session-start");
			session->startSession();
			processEvents();

			CPPUNIT_ASSERT_EQUAL(ClientSession::Error, session->getState());
			CPPUNIT_ASSERT_EQUAL(ClientSession::SessionStartError, *session->getError());
		}

		void testSessionStart_AfterResourceBind() {
			boost::shared_ptr<MockSession> session(createSession("me@foo.com/Bar"));
			session->onSessionStarted.connect(boost::bind(&ClientSessionTest::setSessionStarted, this));
			getMockServer()->expectStreamStart();
			getMockServer()->sendStreamStart();
			getMockServer()->sendStreamFeaturesWithResourceBindAndSession();
			getMockServer()->expectResourceBind("Bar", "session-bind");
			getMockServer()->sendResourceBindResponse("me@foo.com/Bar", "session-bind");
			getMockServer()->expectSessionStart("session-start");
			getMockServer()->sendSessionStartResponse("session-start");
			session->startSession();
			processEvents();

			CPPUNIT_ASSERT_EQUAL(ClientSession::SessionStarted, session->getState());
			CPPUNIT_ASSERT(sessionStarted_);
		}

		void testWhitespacePing() {
			boost::shared_ptr<MockSession> session(createSession("me@foo.com/Bar"));
			getMockServer()->expectStreamStart();
			getMockServer()->sendStreamStart();
			getMockServer()->sendStreamFeatures();
			session->startSession();
			processEvents();
			CPPUNIT_ASSERT(session->getWhitespacePingLayer());
		}

		void testReceiveElementAfterSessionStarted() {
			boost::shared_ptr<MockSession> session(createSession("me@foo.com/Bar"));
			getMockServer()->expectStreamStart();
			getMockServer()->sendStreamStart();
			getMockServer()->sendStreamFeatures();
			session->startSession();
			processEvents();

			getMockServer()->expectMessage();
			session->sendElement(boost::make_shared<Message>()));
		}

		void testSendElement() {
			boost::shared_ptr<MockSession> session(createSession("me@foo.com/Bar"));
			session->onElementReceived.connect(boost::bind(&ClientSessionTest::addReceivedElement, this, _1));
			getMockServer()->expectStreamStart();
			getMockServer()->sendStreamStart();
			getMockServer()->sendStreamFeatures();
			getMockServer()->sendMessage();
			session->startSession();
			processEvents();

			CPPUNIT_ASSERT_EQUAL(1, static_cast<int>(receivedElements_.size()));
			CPPUNIT_ASSERT(boost::dynamic_pointer_cast<Message>(receivedElements_[0]));
		}
#endif
