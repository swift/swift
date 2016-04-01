/*
 * Copyright (c) 2015-2016 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

 /*
    This file uses http://www.tls-o-matic.com/ to test the currently configured TLS backend for correct certificate validation behavior.
 */

#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/extensions/TestFactoryRegistry.h>

#include <Swiften/Base/Log.h>
#include <Swiften/EventLoop/DummyEventLoop.h>
#include <Swiften/IDN/IDNConverter.h>
#include <Swiften/IDN/PlatformIDNConverter.h>
#include <Swiften/Network/BoostConnectionFactory.h>
#include <Swiften/Network/BoostIOServiceThread.h>
#include <Swiften/Network/HostAddressPort.h>
#include <Swiften/Network/PlatformDomainNameResolver.h>
#include <Swiften/Network/TLSConnection.h>
#include <Swiften/Network/TLSConnectionFactory.h>
#include <Swiften/TLS/CertificateVerificationError.h>
#include <Swiften/TLS/PlatformTLSFactories.h>
#include <Swiften/TLS/TLSContext.h>
#include <Swiften/TLS/TLSContextFactory.h>

using namespace Swift;

class CertificateErrorTest : public CppUnit::TestFixture {
        CPPUNIT_TEST_SUITE(CertificateErrorTest);

        // These test require the TLS-O-Matic testing CA to be trusted. For more info see https://www.tls-o-matic.com/https/test1 .
        CPPUNIT_TEST(testTLS_O_MaticTrusted);
        CPPUNIT_TEST(testTLS_O_MaticCertificateFromTheFuture);
        CPPUNIT_TEST(testTLS_O_MaticCertificateFromThePast);
        CPPUNIT_TEST(testTLS_O_MaticCertificateFromUnknownCA);
        CPPUNIT_TEST(testTLS_O_MaticCertificateWrongPurpose);

#if !defined(HAVE_OPENSSL)
        // Our OpenSSL backend does not support revocation. We excluded it from the revocation tests.
        CPPUNIT_TEST(testRevokedCertificateRevocationDisabled);
        CPPUNIT_TEST(testRevokedCertificateRevocationEnabled);
#endif

        CPPUNIT_TEST_SUITE_END();

    public:
        void setUp() {
            eventLoop_ = new DummyEventLoop();
            boostIOServiceThread_ = new BoostIOServiceThread();
            boostIOService_ = std::make_shared<boost::asio::io_service>();
            connectionFactory_ = new BoostConnectionFactory(boostIOServiceThread_->getIOService(), eventLoop_);
            idnConverter_ = PlatformIDNConverter::create();
            domainNameResolver_ = new PlatformDomainNameResolver(idnConverter_, eventLoop_);

            tlsFactories_ = new PlatformTLSFactories();
            tlsContextFactory_ = tlsFactories_->getTLSContextFactory();

            tlsContextFactory_->setCheckCertificateRevocation(false);

            tlsConnectionFactory_ = new TLSConnectionFactory(tlsContextFactory_, connectionFactory_, TLSOptions());

            connectFinished_ = false;
            connectFinishedWithError_ = false;
        }

        void tearDown() {
            delete tlsConnectionFactory_;
            delete tlsFactories_;

            delete domainNameResolver_;
            delete idnConverter_;
            delete connectionFactory_;
            delete boostIOServiceThread_;
            while (eventLoop_->hasEvents()) {
                eventLoop_->processEvents();
            }
            delete eventLoop_;
        }

        HostAddress resolveName(const std::string& name) {
            std::shared_ptr<DomainNameAddressQuery> query = domainNameResolver_->createAddressQuery(name);
            query->onResult.connect(boost::bind(&CertificateErrorTest::handleAddressQueryResult, this, _1, _2));
            lastResoverResult_ = HostAddress();
            resolvingDone_ = false;

            query->run();
            while(!resolvingDone_) {
                eventLoop_->processEvents();
            }

            return lastResoverResult_;
        }

        void connectToServer(std::shared_ptr<TLSConnection> connection, const std::string& hostname, int port) {
            connection->onConnectFinished.connect(boost::bind(&CertificateErrorTest::handleConnectFinished, this, _1));

            HostAddress address = resolveName(hostname);

            connection->connect(HostAddressPort(address, port));

            while (!connectFinished_) {
                eventLoop_->processEvents();
            }
        }

        void testTLS_O_MaticTrusted() {
            std::shared_ptr<TLSConnection> connection = std::dynamic_pointer_cast<TLSConnection>(tlsConnectionFactory_->createConnection());
            TLSContext* context = connection->getTLSContext();

            connectToServer(connection, "test1.tls-o-matic.com", 443);

            CPPUNIT_ASSERT_EQUAL(false, connectFinishedWithError_);
            CPPUNIT_ASSERT_EQUAL(CertificateVerificationError::ref(), context->getPeerCertificateVerificationError());
        }

        void testTLS_O_MaticCertificateFromTheFuture() {
            std::shared_ptr<TLSConnection> connection = std::dynamic_pointer_cast<TLSConnection>(tlsConnectionFactory_->createConnection());
            TLSContext* context = connection->getTLSContext();

            connectToServer(connection, "test5.tls-o-matic.com", 405);

            CPPUNIT_ASSERT_EQUAL(false, connectFinishedWithError_);
            CPPUNIT_ASSERT(context->getPeerCertificateVerificationError());
#if defined(HAVE_SCHANNEL)
            // Windows SChannel API does not differentiate between expired and not yet valid.
            CPPUNIT_ASSERT_EQUAL(CertificateVerificationError::Expired, context->getPeerCertificateVerificationError()->getType());
#else
            CPPUNIT_ASSERT_EQUAL(CertificateVerificationError::NotYetValid, context->getPeerCertificateVerificationError()->getType());
#endif
        }

        void testTLS_O_MaticCertificateFromThePast() {
            std::shared_ptr<TLSConnection> connection = std::dynamic_pointer_cast<TLSConnection>(tlsConnectionFactory_->createConnection());
            TLSContext* context = connection->getTLSContext();

            connectToServer(connection, "test6.tls-o-matic.com", 406);

            CPPUNIT_ASSERT_EQUAL(false, connectFinishedWithError_);
            CPPUNIT_ASSERT(context->getPeerCertificateVerificationError());
            CPPUNIT_ASSERT_EQUAL(CertificateVerificationError::Expired, context->getPeerCertificateVerificationError()->getType());
        }

        void testTLS_O_MaticCertificateFromUnknownCA() {
            std::shared_ptr<TLSConnection> connection = std::dynamic_pointer_cast<TLSConnection>(tlsConnectionFactory_->createConnection());
            TLSContext* context = connection->getTLSContext();

            connectToServer(connection, "test7.tls-o-matic.com", 407);

            CPPUNIT_ASSERT_EQUAL(false, connectFinishedWithError_);
            CPPUNIT_ASSERT(context->getPeerCertificateVerificationError());
            CPPUNIT_ASSERT_EQUAL(CertificateVerificationError::Untrusted, context->getPeerCertificateVerificationError()->getType());
        }

        // test14.tls-o-matic.com:414
        void testTLS_O_MaticCertificateWrongPurpose() {
            std::shared_ptr<TLSConnection> connection = std::dynamic_pointer_cast<TLSConnection>(tlsConnectionFactory_->createConnection());
            TLSContext* context = connection->getTLSContext();

            connectToServer(connection, "test14.tls-o-matic.com", 414);

            CPPUNIT_ASSERT_EQUAL(false, connectFinishedWithError_);
            CPPUNIT_ASSERT(context->getPeerCertificateVerificationError());
            CPPUNIT_ASSERT_EQUAL(CertificateVerificationError::InvalidPurpose, context->getPeerCertificateVerificationError()->getType());
        }

        void testRevokedCertificateRevocationDisabled() {
            tlsContextFactory_->setCheckCertificateRevocation(false);
            std::shared_ptr<TLSConnection> connection = std::dynamic_pointer_cast<TLSConnection>(tlsConnectionFactory_->createConnection());
            TLSContext* context = connection->getTLSContext();

            connectToServer(connection, "revoked.grc.com", 443);

            CPPUNIT_ASSERT_EQUAL(false, connectFinishedWithError_);
            CPPUNIT_ASSERT(!context->getPeerCertificateVerificationError());
        }

        void testRevokedCertificateRevocationEnabled() {
            tlsContextFactory_->setCheckCertificateRevocation(true);
            std::shared_ptr<TLSConnection> connection = std::dynamic_pointer_cast<TLSConnection>(tlsConnectionFactory_->createConnection());
            TLSContext* context = connection->getTLSContext();

            connectToServer(connection, "revoked.grc.com", 443);

            CPPUNIT_ASSERT_EQUAL(false, connectFinishedWithError_);
            CPPUNIT_ASSERT(context->getPeerCertificateVerificationError());
            CPPUNIT_ASSERT_EQUAL(CertificateVerificationError::Revoked, context->getPeerCertificateVerificationError()->getType());
        }

    private:
        void handleAddressQueryResult(const std::vector<HostAddress>& address, boost::optional<DomainNameResolveError> /* error */) {
            if (address.size() > 0) {
                lastResoverResult_ = address[0];
            }
            resolvingDone_ = true;
        }

        void handleConnectFinished(bool error) {
            connectFinished_ = true;
            connectFinishedWithError_ = error;
        }

    private:
        BoostIOServiceThread* boostIOServiceThread_;
        std::shared_ptr<boost::asio::io_service> boostIOService_;
        DummyEventLoop* eventLoop_;
        ConnectionFactory* connectionFactory_;
        PlatformTLSFactories* tlsFactories_;
        TLSContextFactory* tlsContextFactory_;
        TLSConnectionFactory* tlsConnectionFactory_;

        IDNConverter* idnConverter_;
        DomainNameResolver* domainNameResolver_;
        HostAddress lastResoverResult_;
        bool resolvingDone_;

        bool connectFinished_;
        bool connectFinishedWithError_;
};


CPPUNIT_TEST_SUITE_REGISTRATION(CertificateErrorTest);
