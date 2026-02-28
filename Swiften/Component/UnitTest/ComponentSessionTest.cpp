/*
 * Copyright (c) 2010-2016 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#include <deque>

#include <boost/bind.hpp>
#include <boost/optional.hpp>

#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/extensions/TestFactoryRegistry.h>

#include <Swiften/Component/ComponentSession.h>
#include <Swiften/Crypto/CryptoProvider.h>
#include <Swiften/Crypto/PlatformCryptoProvider.h>
#include <Swiften/Elements/AuthFailure.h>
#include <Swiften/Elements/ComponentHandshake.h>
#include <Swiften/Session/SessionStream.h>

using namespace Swift;

class ComponentSessionTest : public CppUnit::TestFixture {
        CPPUNIT_TEST_SUITE(ComponentSessionTest);
        CPPUNIT_TEST(testStart);
        CPPUNIT_TEST(testStart_Error);
        CPPUNIT_TEST(testStart_Unauthorized);
        CPPUNIT_TEST_SUITE_END();

    public:
        void setUp() {
            server = std::make_shared<MockSessionStream>();
            sessionFinishedReceived = false;
            crypto = std::shared_ptr<CryptoProvider>(PlatformCryptoProvider::create());
        }

        void testStart() {
            std::shared_ptr<ComponentSession> session(createSession());
            session->start();
            server->receiveStreamStart();
            server->sendStreamStart();
            server->receiveHandshake();
            server->sendHandshakeResponse();

            CPPUNIT_ASSERT(server->whitespacePingEnabled);

            session->finish();
            CPPUNIT_ASSERT(!server->whitespacePingEnabled);

        }

        void testStart_Error() {
            std::shared_ptr<ComponentSession> session(createSession());
            session->start();
            server->breakConnection();

            CPPUNIT_ASSERT_EQUAL(ComponentSession::Finished, session->getState());
            CPPUNIT_ASSERT(sessionFinishedReceived);
            CPPUNIT_ASSERT(sessionFinishedError);
        }

        void testStart_Unauthorized() {
            std::shared_ptr<ComponentSession> session(createSession());
            session->start();
            server->receiveStreamStart();
            server->sendStreamStart();
            server->receiveHandshake();
            server->sendHandshakeError();

            CPPUNIT_ASSERT_EQUAL(ComponentSession::Finished, session->getState());
            CPPUNIT_ASSERT(sessionFinishedReceived);
            CPPUNIT_ASSERT(sessionFinishedError);
        }

    private:
        std::shared_ptr<ComponentSession> createSession() {
            std::shared_ptr<ComponentSession> session = ComponentSession::create(JID("service.foo.com"), "servicesecret", server, crypto.get());
            session->onFinished.connect(boost::bind(&ComponentSessionTest::handleSessionFinished, this, _1));
            return session;
        }

        void handleSessionFinished(std::shared_ptr<Error> error) {
            sessionFinishedReceived = true;
            sessionFinishedError = error;
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

                MockSessionStream() : available(true), whitespacePingEnabled(false), resetCount(0) {
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
                    return false;
                }

                virtual void addTLSEncryption() {
                    assert(false);
                }

                virtual bool isTLSEncrypted() {
                    return false;
                }

                virtual ByteArray getTLSFinishMessage() const {
                    return ByteArray();
                }

                virtual Certificate::ref getPeerCertificate() const {
                    return Certificate::ref();
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
                    assert(false);
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

                void sendStreamStart() {
                    ProtocolHeader header;
                    header.setFrom("service.foo.com");
                    return onStreamStartReceived(header);
                }

                void sendHandshakeResponse() {
                    onElementReceived(ComponentHandshake::ref(new ComponentHandshake()));
                }

                void sendHandshakeError() {
                    // FIXME: This isn't the correct element
                    onElementReceived(AuthFailure::ref(new AuthFailure()));
                }

                void receiveStreamStart() {
                    Event event = popEvent();
                    CPPUNIT_ASSERT(event.header);
                }

                void receiveHandshake() {
                    Event event = popEvent();
                    CPPUNIT_ASSERT(event.element);
                    ComponentHandshake::ref handshake(std::dynamic_pointer_cast<ComponentHandshake>(event.element));
                    CPPUNIT_ASSERT(handshake);
                    CPPUNIT_ASSERT_EQUAL(std::string("4c4f8a41141722c8bbfbdd92d827f7b2fc0a542b"), handshake->getData());
                }

                Event popEvent() {
                    CPPUNIT_ASSERT(!receivedEvents.empty());
                    Event event = receivedEvents.front();
                    receivedEvents.pop_front();
                    return event;
                }

                bool available;
                bool whitespacePingEnabled;
                std::string bindID;
                int resetCount;
                std::deque<Event> receivedEvents;
        };

        std::shared_ptr<MockSessionStream> server;
        bool sessionFinishedReceived = false;
        std::shared_ptr<Error> sessionFinishedError;
        std::shared_ptr<CryptoProvider> crypto;
};

CPPUNIT_TEST_SUITE_REGISTRATION(ComponentSessionTest);
