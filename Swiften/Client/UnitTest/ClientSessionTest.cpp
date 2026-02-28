/*
 * Copyright (c) 2010-2016 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#include <deque>
#include <memory>

#include <boost/bind.hpp>
#include <boost/optional.hpp>

#include <Swiften/Base/Debug.h>

#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/extensions/TestFactoryRegistry.h>

#include <Swiften/Client/ClientSession.h>
#include <Swiften/Crypto/CryptoProvider.h>
#include <Swiften/Crypto/PlatformCryptoProvider.h>
#include <Swiften/Elements/AuthChallenge.h>
#include <Swiften/Elements/AuthFailure.h>
#include <Swiften/Elements/AuthRequest.h>
#include <Swiften/Elements/AuthSuccess.h>
#include <Swiften/Elements/EnableStreamManagement.h>
#include <Swiften/Elements/IQ.h>
#include <Swiften/Elements/Message.h>
#include <Swiften/Elements/ResourceBind.h>
#include <Swiften/Elements/StanzaAck.h>
#include <Swiften/Elements/StartTLSFailure.h>
#include <Swiften/Elements/StartTLSRequest.h>
#include <Swiften/Elements/StreamError.h>
#include <Swiften/Elements/StreamFeatures.h>
#include <Swiften/Elements/StreamManagementEnabled.h>
#include <Swiften/Elements/StreamManagementFailed.h>
#include <Swiften/Elements/TLSProceed.h>
#include <Swiften/IDN/IDNConverter.h>
#include <Swiften/IDN/PlatformIDNConverter.h>
#include <Swiften/Network/DummyTimerFactory.h>
#include <Swiften/Session/SessionStream.h>
#include <Swiften/TLS/BlindCertificateTrustChecker.h>
#include <Swiften/TLS/SimpleCertificate.h>

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
        CPPUNIT_TEST(testAuthenticate_EXTERNAL);
        CPPUNIT_TEST(testStreamManagement);
        CPPUNIT_TEST(testStreamManagement_Failed);
        CPPUNIT_TEST(testUnexpectedChallenge);
        CPPUNIT_TEST(testFinishAcksStanzas);

        CPPUNIT_TEST(testServerInitiatedSessionClose);
        CPPUNIT_TEST(testClientInitiatedSessionClose);
        CPPUNIT_TEST(testTimeoutOnShutdown);

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
            crypto = std::shared_ptr<CryptoProvider>(PlatformCryptoProvider::create());
            idnConverter = std::shared_ptr<IDNConverter>(PlatformIDNConverter::create());
            timerFactory = std::make_shared<DummyTimerFactory>();
            server = std::make_shared<MockSessionStream>();
            sessionFinishedReceived = false;
            needCredentials = false;
            blindCertificateTrustChecker = new BlindCertificateTrustChecker();
        }

        void tearDown() {
            delete blindCertificateTrustChecker;
        }

        void testStart_Error() {
            std::shared_ptr<ClientSession> session(createSession());
            session->start();
            server->breakConnection();

            CPPUNIT_ASSERT_EQUAL(ClientSession::State::Finished, session->getState());
            CPPUNIT_ASSERT(sessionFinishedReceived);
            CPPUNIT_ASSERT(sessionFinishedError);
        }

        void testStart_StreamError() {
            std::shared_ptr<ClientSession> session(createSession());
            session->start();
            server->sendStreamStart();
            server->sendStreamError();

            CPPUNIT_ASSERT_EQUAL(ClientSession::State::Finishing, session->getState());
            CPPUNIT_ASSERT_EQUAL(true, server->receivedEvents.back().footer);

            server->onStreamEndReceived();
            CPPUNIT_ASSERT_EQUAL(ClientSession::State::Finished, session->getState());
            CPPUNIT_ASSERT(sessionFinishedReceived);
            CPPUNIT_ASSERT(sessionFinishedError);
        }

        void testStartTLS() {
            std::shared_ptr<ClientSession> session(createSession());
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
            std::shared_ptr<ClientSession> session(createSession());
            session->start();
            server->receiveStreamStart();
            server->sendStreamStart();
            server->sendStreamFeaturesWithStartTLS();
            server->receiveStartTLS();
            server->sendTLSFailure();

            CPPUNIT_ASSERT(!server->tlsEncrypted);
            CPPUNIT_ASSERT_EQUAL(ClientSession::State::Finishing, session->getState());
            CPPUNIT_ASSERT_EQUAL(true, server->receivedEvents.back().footer);

            server->onStreamEndReceived();
            CPPUNIT_ASSERT_EQUAL(ClientSession::State::Finished, session->getState());
            CPPUNIT_ASSERT(sessionFinishedReceived);
            CPPUNIT_ASSERT(sessionFinishedError);
        }

        void testStartTLS_ConnectError() {
            std::shared_ptr<ClientSession> session(createSession());
            session->start();
            server->receiveStreamStart();
            server->sendStreamStart();
            server->sendStreamFeaturesWithStartTLS();
            server->receiveStartTLS();
            server->sendTLSProceed();
            server->breakTLS();

            CPPUNIT_ASSERT_EQUAL(ClientSession::State::Finished, session->getState());
            CPPUNIT_ASSERT(sessionFinishedReceived);
            CPPUNIT_ASSERT(sessionFinishedError);
        }

        void testStartTLS_InvalidIdentity() {
            std::shared_ptr<ClientSession> session(createSession());
            session->start();
            server->receiveStreamStart();
            server->sendStreamStart();
            server->sendStreamFeaturesWithStartTLS();
            server->receiveStartTLS();
            CPPUNIT_ASSERT(!server->tlsEncrypted);
            server->sendTLSProceed();
            CPPUNIT_ASSERT(server->tlsEncrypted);
            server->onTLSEncrypted();
            server->close();
            CPPUNIT_ASSERT_EQUAL(ClientSession::State::Finished, session->getState());

            CPPUNIT_ASSERT(sessionFinishedReceived);
            CPPUNIT_ASSERT(sessionFinishedError);
            CPPUNIT_ASSERT(std::dynamic_pointer_cast<CertificateVerificationError>(sessionFinishedError));
            CPPUNIT_ASSERT_EQUAL(CertificateVerificationError::InvalidServerIdentity, std::dynamic_pointer_cast<CertificateVerificationError>(sessionFinishedError)->getType());
        }

        void testStart_StreamFeaturesWithoutResourceBindingFails() {
            std::shared_ptr<ClientSession> session(createSession());
            session->start();
            server->receiveStreamStart();
            server->sendStreamStart();
            server->sendEmptyStreamFeatures();

            CPPUNIT_ASSERT_EQUAL(ClientSession::State::Finishing, session->getState());
            CPPUNIT_ASSERT_EQUAL(true, server->receivedEvents.back().footer);

            server->onStreamEndReceived();
            CPPUNIT_ASSERT_EQUAL(ClientSession::State::Finished, session->getState());
            CPPUNIT_ASSERT(sessionFinishedReceived);
            CPPUNIT_ASSERT(sessionFinishedError);
        }

        void testAuthenticate() {
            std::shared_ptr<ClientSession> session(createSession());
            session->start();
            server->receiveStreamStart();
            server->sendStreamStart();
            server->sendStreamFeaturesWithPLAINAuthentication();
            CPPUNIT_ASSERT(needCredentials);
            CPPUNIT_ASSERT_EQUAL(ClientSession::State::WaitingForCredentials, session->getState());
            session->sendCredentials(createSafeByteArray("mypass"));
            server->receiveAuthRequest("PLAIN");
            server->sendAuthSuccess();
            server->receiveStreamStart();

            session->finish();
        }

        void testAuthenticate_Unauthorized() {
            std::shared_ptr<ClientSession> session(createSession());
            session->start();
            server->receiveStreamStart();
            server->sendStreamStart();
            server->sendStreamFeaturesWithPLAINAuthentication();
            CPPUNIT_ASSERT(needCredentials);
            CPPUNIT_ASSERT_EQUAL(ClientSession::State::WaitingForCredentials, session->getState());
            session->sendCredentials(createSafeByteArray("mypass"));
            server->receiveAuthRequest("PLAIN");
            server->sendAuthFailure();

            CPPUNIT_ASSERT_EQUAL(ClientSession::State::Finishing, session->getState());
            CPPUNIT_ASSERT_EQUAL(true, server->receivedEvents.back().footer);

            server->onStreamEndReceived();
            CPPUNIT_ASSERT_EQUAL(ClientSession::State::Finished, session->getState());
            CPPUNIT_ASSERT(sessionFinishedReceived);
            CPPUNIT_ASSERT(sessionFinishedError);
        }

        void testAuthenticate_PLAINOverNonTLS() {
            std::shared_ptr<ClientSession> session(createSession());
            session->setAllowPLAINOverNonTLS(false);
            session->start();
            server->receiveStreamStart();
            server->sendStreamStart();
            server->sendStreamFeaturesWithPLAINAuthentication();

            CPPUNIT_ASSERT_EQUAL(ClientSession::State::Finishing, session->getState());
            CPPUNIT_ASSERT_EQUAL(true, server->receivedEvents.back().footer);

            server->onStreamEndReceived();
            CPPUNIT_ASSERT_EQUAL(ClientSession::State::Finished, session->getState());
            CPPUNIT_ASSERT(sessionFinishedReceived);
            CPPUNIT_ASSERT(sessionFinishedError);
        }

        void testAuthenticate_RequireTLS() {
            std::shared_ptr<ClientSession> session(createSession());
            session->setUseTLS(ClientSession::RequireTLS);
            session->setAllowPLAINOverNonTLS(true);
            session->start();
            server->receiveStreamStart();
            server->sendStreamStart();
            server->sendStreamFeaturesWithMultipleAuthentication();
            CPPUNIT_ASSERT_EQUAL(ClientSession::State::Finishing, session->getState());
            CPPUNIT_ASSERT_EQUAL(true, server->receivedEvents.back().footer);

            server->onStreamEndReceived();
            CPPUNIT_ASSERT_EQUAL(ClientSession::State::Finished, session->getState());
            CPPUNIT_ASSERT(sessionFinishedReceived);
            CPPUNIT_ASSERT(sessionFinishedError);
        }

        void testAuthenticate_NoValidAuthMechanisms() {
            std::shared_ptr<ClientSession> session(createSession());
            session->start();
            server->receiveStreamStart();
            server->sendStreamStart();
            server->sendStreamFeaturesWithUnknownAuthentication();

            CPPUNIT_ASSERT_EQUAL(ClientSession::State::Finishing, session->getState());
            CPPUNIT_ASSERT_EQUAL(true, server->receivedEvents.back().footer);

            server->onStreamEndReceived();
            server->close();
            CPPUNIT_ASSERT_EQUAL(ClientSession::State::Finished, session->getState());
            CPPUNIT_ASSERT(sessionFinishedReceived);
            CPPUNIT_ASSERT(sessionFinishedError);
        }

        void testAuthenticate_EXTERNAL() {
            std::shared_ptr<ClientSession> session(createSession());
            session->start();
            server->receiveStreamStart();
            server->sendStreamStart();
            server->sendStreamFeaturesWithEXTERNALAuthentication();
            server->receiveAuthRequest("EXTERNAL");
            server->sendAuthSuccess();
            server->receiveStreamStart();

            session->finish();
        }

        void testUnexpectedChallenge() {
            std::shared_ptr<ClientSession> session(createSession());
            session->start();
            server->receiveStreamStart();
            server->sendStreamStart();
            server->sendStreamFeaturesWithEXTERNALAuthentication();
            server->receiveAuthRequest("EXTERNAL");
            server->sendChallenge();
            server->sendChallenge();

            CPPUNIT_ASSERT_EQUAL(ClientSession::State::Finishing, session->getState());
            CPPUNIT_ASSERT_EQUAL(true, server->receivedEvents.back().footer);

            server->onStreamEndReceived();
            CPPUNIT_ASSERT_EQUAL(ClientSession::State::Finished, session->getState());
            CPPUNIT_ASSERT(sessionFinishedReceived);
            CPPUNIT_ASSERT(sessionFinishedError);
        }

        void testStreamManagement() {
            std::shared_ptr<ClientSession> session(createSession());
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
            CPPUNIT_ASSERT_EQUAL(ClientSession::State::Initialized, session->getState());

            session->finish();
        }

        void testStreamManagement_Failed() {
            std::shared_ptr<ClientSession> session(createSession());
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
            CPPUNIT_ASSERT_EQUAL(ClientSession::State::Initialized, session->getState());

            session->finish();
        }

        void testFinishAcksStanzas() {
            std::shared_ptr<ClientSession> session(createSession());
            initializeSession(session);
            server->sendMessage();
            server->sendMessage();
            server->sendMessage();

            session->finish();

            server->receiveAck(3);
        }

        void testServerInitiatedSessionClose() {
            std::shared_ptr<ClientSession> session(createSession());
            initializeSession(session);

            server->onStreamEndReceived();
            server->close();
            CPPUNIT_ASSERT_EQUAL(ClientSession::State::Finished, session->getState());
            CPPUNIT_ASSERT_EQUAL(true, server->receivedEvents.back().footer);
        }

        void testClientInitiatedSessionClose() {
            std::shared_ptr<ClientSession> session(createSession());
            initializeSession(session);

            session->finish();
            CPPUNIT_ASSERT_EQUAL(ClientSession::State::Finishing, session->getState());
            CPPUNIT_ASSERT_EQUAL(true, server->receivedEvents.back().footer);

            server->onStreamEndReceived();
            CPPUNIT_ASSERT_EQUAL(ClientSession::State::Finished, session->getState());
        }

        void testTimeoutOnShutdown() {
            std::shared_ptr<ClientSession> session(createSession());
            initializeSession(session);

            session->finish();
            CPPUNIT_ASSERT_EQUAL(ClientSession::State::Finishing, session->getState());
            CPPUNIT_ASSERT_EQUAL(true, server->receivedEvents.back().footer);
            timerFactory->setTime(60000);

            CPPUNIT_ASSERT_EQUAL(ClientSession::State::Finished, session->getState());
            CPPUNIT_ASSERT(sessionFinishedReceived);
        }

    private:
        std::shared_ptr<ClientSession> createSession() {
            std::shared_ptr<ClientSession> session = ClientSession::create(JID("me@foo.com"), server, idnConverter.get(), crypto.get(), timerFactory.get());
            session->onFinished.connect(boost::bind(&ClientSessionTest::handleSessionFinished, this, _1));
            session->onNeedCredentials.connect(boost::bind(&ClientSessionTest::handleSessionNeedCredentials, this));
            session->setAllowPLAINOverNonTLS(true);
            return session;
        }

        void initializeSession(std::shared_ptr<ClientSession> session) {
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

        void handleSessionFinished(std::shared_ptr<Error> error) {
            sessionFinishedReceived = true;
            sessionFinishedError = error;
        }

        void handleSessionNeedCredentials() {
            needCredentials = true;
        }

        class MockSessionStream : public SessionStream {
            public:
                struct Event {
                    Event(std::shared_ptr<ToplevelElement> element) : element(element), footer(false) {}
                    Event(const ProtocolHeader& header) : header(header), footer(false) {}
                    Event() : footer(true) {}

                    std::shared_ptr<ToplevelElement> element;
                    boost::optional<ProtocolHeader> header;
                    bool footer;
                };

                MockSessionStream() : available(true), canTLSEncrypt(true), tlsEncrypted(false), compressed(false), whitespacePingEnabled(false), resetCount(0) {
                }

                virtual void close() {
                    onClosed(std::shared_ptr<Error>());
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

                virtual void writeElement(std::shared_ptr<ToplevelElement> element) {
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

                virtual std::vector<Certificate::ref> getPeerCertificateChain() const {
                     return std::vector<Certificate::ref>();
                }

                virtual std::shared_ptr<CertificateVerificationError> getPeerCertificateVerificationError() const {
                    return std::shared_ptr<CertificateVerificationError>();
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
                    onClosed(std::make_shared<SessionStream::SessionStreamError>(SessionStream::SessionStreamError::ConnectionReadError));
                }

                void breakTLS() {
                    onClosed(std::make_shared<SessionStream::SessionStreamError>(SessionStream::SessionStreamError::TLSError));
                }


                void sendStreamStart() {
                    ProtocolHeader header;
                    header.setTo("foo.com");
                    return onStreamStartReceived(header);
                }

                void sendStreamFeaturesWithStartTLS() {
                    std::shared_ptr<StreamFeatures> streamFeatures(new StreamFeatures());
                    streamFeatures->setHasStartTLS();
                    onElementReceived(streamFeatures);
                }

                void sendChallenge() {
                    onElementReceived(std::make_shared<AuthChallenge>());
                }

                void sendStreamError() {
                    onElementReceived(std::make_shared<StreamError>());
                }

                void sendTLSProceed() {
                    onElementReceived(std::make_shared<TLSProceed>());
                }

                void sendTLSFailure() {
                    onElementReceived(std::make_shared<StartTLSFailure>());
                }

                void sendStreamFeaturesWithMultipleAuthentication() {
                    std::shared_ptr<StreamFeatures> streamFeatures(new StreamFeatures());
                    streamFeatures->addAuthenticationMechanism("PLAIN");
                    streamFeatures->addAuthenticationMechanism("DIGEST-MD5");
                    streamFeatures->addAuthenticationMechanism("SCRAM-SHA1");
                    onElementReceived(streamFeatures);
                }

                void sendStreamFeaturesWithPLAINAuthentication() {
                    std::shared_ptr<StreamFeatures> streamFeatures(new StreamFeatures());
                    streamFeatures->addAuthenticationMechanism("PLAIN");
                    onElementReceived(streamFeatures);
                }

                void sendStreamFeaturesWithEXTERNALAuthentication() {
                    std::shared_ptr<StreamFeatures> streamFeatures(new StreamFeatures());
                    streamFeatures->addAuthenticationMechanism("EXTERNAL");
                    onElementReceived(streamFeatures);
                }

                void sendStreamFeaturesWithUnknownAuthentication() {
                    std::shared_ptr<StreamFeatures> streamFeatures(new StreamFeatures());
                    streamFeatures->addAuthenticationMechanism("UNKNOWN");
                    onElementReceived(streamFeatures);
                }

                void sendStreamFeaturesWithBindAndStreamManagement() {
                    std::shared_ptr<StreamFeatures> streamFeatures(new StreamFeatures());
                    streamFeatures->setHasResourceBind();
                    streamFeatures->setHasStreamManagement();
                    onElementReceived(streamFeatures);
                }

                void sendEmptyStreamFeatures() {
                    onElementReceived(std::make_shared<StreamFeatures>());
                }

                void sendAuthSuccess() {
                    onElementReceived(std::make_shared<AuthSuccess>());
                }

                void sendAuthFailure() {
                    onElementReceived(std::make_shared<AuthFailure>());
                }

                void sendStreamManagementEnabled() {
                    onElementReceived(std::make_shared<StreamManagementEnabled>());
                }

                void sendStreamManagementFailed() {
                    onElementReceived(std::make_shared<StreamManagementFailed>());
                }

                void sendBindResult() {
                    std::shared_ptr<ResourceBind> resourceBind(new ResourceBind());
                    resourceBind->setJID(JID("foo@bar.com/bla"));
                    std::shared_ptr<IQ> iq = IQ::createResult(JID("foo@bar.com"), bindID, resourceBind);
                    onElementReceived(iq);
                }

                void sendMessage() {
                    std::shared_ptr<Message> message = std::make_shared<Message>();
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
                    CPPUNIT_ASSERT(std::dynamic_pointer_cast<StartTLSRequest>(event.element));
                }

                void receiveAuthRequest(const std::string& mech) {
                    Event event = popEvent();
                    CPPUNIT_ASSERT(event.element);
                    std::shared_ptr<AuthRequest> request(std::dynamic_pointer_cast<AuthRequest>(event.element));
                    CPPUNIT_ASSERT(request);
                    CPPUNIT_ASSERT_EQUAL(mech, request->getMechanism());
                }

                void receiveStreamManagementEnable() {
                    Event event = popEvent();
                    CPPUNIT_ASSERT(event.element);
                    CPPUNIT_ASSERT(std::dynamic_pointer_cast<EnableStreamManagement>(event.element));
                }

                void receiveBind() {
                    Event event = popEvent();
                    CPPUNIT_ASSERT(event.element);
                    std::shared_ptr<IQ> iq = std::dynamic_pointer_cast<IQ>(event.element);
                    CPPUNIT_ASSERT(iq);
                    CPPUNIT_ASSERT(iq->getPayload<ResourceBind>());
                    bindID = iq->getID();
                }

                void receiveAck(unsigned int n) {
                    Event event = popEvent();
                    CPPUNIT_ASSERT(event.element);
                    std::shared_ptr<StanzaAck> ack = std::dynamic_pointer_cast<StanzaAck>(event.element);
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

        std::shared_ptr<IDNConverter> idnConverter;
        std::shared_ptr<MockSessionStream> server;
        bool sessionFinishedReceived;
        bool needCredentials;
        std::shared_ptr<Error> sessionFinishedError;
        BlindCertificateTrustChecker* blindCertificateTrustChecker;
        std::shared_ptr<CryptoProvider> crypto;
        std::shared_ptr<DummyTimerFactory> timerFactory;
};

CPPUNIT_TEST_SUITE_REGISTRATION(ClientSessionTest);

#if 0
        void testAuthenticate() {
            std::shared_ptr<MockSession> session(createSession("me@foo.com/Bar"));
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
            std::shared_ptr<MockSession> session(createSession("me@foo.com/Bar"));
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
            std::shared_ptr<MockSession> session(createSession("me@foo.com/Bar"));
            getMockServer()->expectStreamStart();
            getMockServer()->sendStreamStart();
            getMockServer()->sendStreamFeaturesWithUnsupportedAuthentication();
            session->startSession();
            processEvents();

            CPPUNIT_ASSERT_EQUAL(ClientSession::Error, session->getState());
            CPPUNIT_ASSERT_EQUAL(ClientSession::NoSupportedAuthMechanismsError, *session->getError());
        }

        void testResourceBind() {
            std::shared_ptr<MockSession> session(createSession("me@foo.com/Bar"));
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
            std::shared_ptr<MockSession> session(createSession("me@foo.com/Bar"));
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
            std::shared_ptr<MockSession> session(createSession("me@foo.com"));
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
            std::shared_ptr<MockSession> session(createSession("me@foo.com"));
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
            std::shared_ptr<MockSession> session(createSession("me@foo.com/Bar"));
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
            std::shared_ptr<MockSession> session(createSession("me@foo.com/Bar"));
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
            std::shared_ptr<MockSession> session(createSession("me@foo.com/Bar"));
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
            std::shared_ptr<MockSession> session(createSession("me@foo.com/Bar"));
            getMockServer()->expectStreamStart();
            getMockServer()->sendStreamStart();
            getMockServer()->sendStreamFeatures();
            session->startSession();
            processEvents();
            CPPUNIT_ASSERT(session->getWhitespacePingLayer());
        }

        void testReceiveElementAfterSessionStarted() {
            std::shared_ptr<MockSession> session(createSession("me@foo.com/Bar"));
            getMockServer()->expectStreamStart();
            getMockServer()->sendStreamStart();
            getMockServer()->sendStreamFeatures();
            session->startSession();
            processEvents();

            getMockServer()->expectMessage();
            session->sendElement(std::make_shared<Message>()));
        }

        void testSendElement() {
            std::shared_ptr<MockSession> session(createSession("me@foo.com/Bar"));
            session->onElementReceived.connect(boost::bind(&ClientSessionTest::addReceivedElement, this, _1));
            getMockServer()->expectStreamStart();
            getMockServer()->sendStreamStart();
            getMockServer()->sendStreamFeatures();
            getMockServer()->sendMessage();
            session->startSession();
            processEvents();

            CPPUNIT_ASSERT_EQUAL(1, static_cast<int>(receivedElements_.size()));
            CPPUNIT_ASSERT(std::dynamic_pointer_cast<Message>(receivedElements_[0]));
        }
#endif
