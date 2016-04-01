/*
 * Copyright (c) 2010-2016 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#include <vector>

#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/extensions/TestFactoryRegistry.h>

#include <Swiften/Base/ByteArray.h>
#include <Swiften/IDN/IDNConverter.h>
#include <Swiften/IDN/PlatformIDNConverter.h>
#include <Swiften/TLS/ServerIdentityVerifier.h>
#include <Swiften/TLS/SimpleCertificate.h>

using namespace Swift;

class ServerIdentityVerifierTest : public CppUnit::TestFixture {
        CPPUNIT_TEST_SUITE(ServerIdentityVerifierTest);
        CPPUNIT_TEST(testCertificateVerifies_WithoutMatchingDNSName);
        CPPUNIT_TEST(testCertificateVerifies_WithMatchingDNSName);
        CPPUNIT_TEST(testCertificateVerifies_WithSecondMatchingDNSName);
        CPPUNIT_TEST(testCertificateVerifies_WithMatchingInternationalDNSName);
        CPPUNIT_TEST(testCertificateVerifies_WithMatchingDNSNameWithWildcard);
        CPPUNIT_TEST(testCertificateVerifies_WithMatchingDNSNameWithWildcardMatchingNoComponents);
        CPPUNIT_TEST(testCertificateVerifies_WithDNSNameWithWildcardMatchingTwoComponents);
        CPPUNIT_TEST(testCertificateVerifies_WithMatchingSRVNameWithoutService);
        CPPUNIT_TEST(testCertificateVerifies_WithMatchingSRVNameWithService);
        CPPUNIT_TEST(testCertificateVerifies_WithMatchingSRVNameWithServiceAndWildcard);
        CPPUNIT_TEST(testCertificateVerifies_WithMatchingSRVNameWithDifferentService);
        CPPUNIT_TEST(testCertificateVerifies_WithMatchingXmppAddr);
        CPPUNIT_TEST(testCertificateVerifies_WithMatchingXmppAddrWithWildcard);
        CPPUNIT_TEST(testCertificateVerifies_WithMatchingInternationalXmppAddr);
        CPPUNIT_TEST(testCertificateVerifies_WithMatchingCNWithoutSAN);
        CPPUNIT_TEST(testCertificateVerifies_WithMatchingCNWithSAN);
        CPPUNIT_TEST_SUITE_END();

    public:
        void setUp() {
            idnConverter = std::shared_ptr<IDNConverter>(PlatformIDNConverter::create());
        }

        void testCertificateVerifies_WithoutMatchingDNSName() {
            ServerIdentityVerifier testling(JID("foo@bar.com/baz"), idnConverter.get());
            SimpleCertificate::ref certificate(new SimpleCertificate());
            certificate->addDNSName("foo.com");

            CPPUNIT_ASSERT(!testling.certificateVerifies(certificate));
        }

        void testCertificateVerifies_WithMatchingDNSName() {
            ServerIdentityVerifier testling(JID("foo@bar.com/baz"), idnConverter.get());
            SimpleCertificate::ref certificate(new SimpleCertificate());
            certificate->addDNSName("bar.com");

            CPPUNIT_ASSERT(testling.certificateVerifies(certificate));
        }

        void testCertificateVerifies_WithSecondMatchingDNSName() {
            ServerIdentityVerifier testling(JID("foo@bar.com/baz"), idnConverter.get());
            SimpleCertificate::ref certificate(new SimpleCertificate());
            certificate->addDNSName("foo.com");
            certificate->addDNSName("bar.com");

            CPPUNIT_ASSERT(testling.certificateVerifies(certificate));
        }

        void testCertificateVerifies_WithMatchingInternationalDNSName() {
            ServerIdentityVerifier testling(JID("foo@tron\xc3\xa7on.com/baz"), idnConverter.get());
            SimpleCertificate::ref certificate(new SimpleCertificate());
            certificate->addDNSName("xn--tronon-zua.com");

            CPPUNIT_ASSERT(testling.certificateVerifies(certificate));
        }

        void testCertificateVerifies_WithMatchingDNSNameWithWildcard() {
            ServerIdentityVerifier testling(JID("foo@im.bar.com/baz"), idnConverter.get());
            SimpleCertificate::ref certificate(new SimpleCertificate());
            certificate->addDNSName("*.bar.com");

            CPPUNIT_ASSERT(testling.certificateVerifies(certificate));
        }

        void testCertificateVerifies_WithMatchingDNSNameWithWildcardMatchingNoComponents() {
            ServerIdentityVerifier testling(JID("foo@bar.com/baz"), idnConverter.get());
            SimpleCertificate::ref certificate(new SimpleCertificate());
            certificate->addDNSName("*.bar.com");

            CPPUNIT_ASSERT(!testling.certificateVerifies(certificate));
        }

        void testCertificateVerifies_WithDNSNameWithWildcardMatchingTwoComponents() {
            ServerIdentityVerifier testling(JID("foo@xmpp.im.bar.com/baz"), idnConverter.get());
            SimpleCertificate::ref certificate(new SimpleCertificate());
            certificate->addDNSName("*.bar.com");

            CPPUNIT_ASSERT(!testling.certificateVerifies(certificate));
        }

        void testCertificateVerifies_WithMatchingSRVNameWithoutService() {
            ServerIdentityVerifier testling(JID("foo@bar.com/baz"), idnConverter.get());
            SimpleCertificate::ref certificate(new SimpleCertificate());
            certificate->addSRVName("bar.com");

            CPPUNIT_ASSERT(!testling.certificateVerifies(certificate));
        }

        void testCertificateVerifies_WithMatchingSRVNameWithService() {
            ServerIdentityVerifier testling(JID("foo@bar.com/baz"), idnConverter.get());
            SimpleCertificate::ref certificate(new SimpleCertificate());
            certificate->addSRVName("_xmpp-client.bar.com");

            CPPUNIT_ASSERT(testling.certificateVerifies(certificate));
        }

        void testCertificateVerifies_WithMatchingSRVNameWithServiceAndWildcard() {
            ServerIdentityVerifier testling(JID("foo@im.bar.com/baz"), idnConverter.get());
            SimpleCertificate::ref certificate(new SimpleCertificate());
            certificate->addSRVName("_xmpp-client.*.bar.com");

            CPPUNIT_ASSERT(testling.certificateVerifies(certificate));
        }

        void testCertificateVerifies_WithMatchingSRVNameWithDifferentService() {
            ServerIdentityVerifier testling(JID("foo@bar.com/baz"), idnConverter.get());
            SimpleCertificate::ref certificate(new SimpleCertificate());
            certificate->addSRVName("_xmpp-server.bar.com");

            CPPUNIT_ASSERT(!testling.certificateVerifies(certificate));
        }

        void testCertificateVerifies_WithMatchingXmppAddr() {
            ServerIdentityVerifier testling(JID("foo@bar.com/baz"), idnConverter.get());
            SimpleCertificate::ref certificate(new SimpleCertificate());
            certificate->addXMPPAddress("bar.com");

            CPPUNIT_ASSERT(testling.certificateVerifies(certificate));
        }

        void testCertificateVerifies_WithMatchingXmppAddrWithWildcard() {
            ServerIdentityVerifier testling(JID("foo@im.bar.com/baz"), idnConverter.get());
            SimpleCertificate::ref certificate(new SimpleCertificate());
            certificate->addXMPPAddress("*.bar.com");

            CPPUNIT_ASSERT(!testling.certificateVerifies(certificate));
        }

        void testCertificateVerifies_WithMatchingInternationalXmppAddr() {
            ServerIdentityVerifier testling(JID("foo@tron\xc3\xa7.com/baz"), idnConverter.get());
            SimpleCertificate::ref certificate(new SimpleCertificate());
            certificate->addXMPPAddress("tron\xc3\xa7.com");

            CPPUNIT_ASSERT(testling.certificateVerifies(certificate));
        }

        void testCertificateVerifies_WithMatchingCNWithoutSAN() {
            ServerIdentityVerifier testling(JID("foo@bar.com/baz"), idnConverter.get());
            SimpleCertificate::ref certificate(new SimpleCertificate());
            certificate->addCommonName("bar.com");

            CPPUNIT_ASSERT(testling.certificateVerifies(certificate));
        }

        void testCertificateVerifies_WithMatchingCNWithSAN() {
            ServerIdentityVerifier testling(JID("foo@bar.com/baz"), idnConverter.get());
            SimpleCertificate::ref certificate(new SimpleCertificate());
            certificate->addSRVName("foo.com");
            certificate->addCommonName("bar.com");

            CPPUNIT_ASSERT(!testling.certificateVerifies(certificate));
        }

        std::shared_ptr<IDNConverter> idnConverter;
};

CPPUNIT_TEST_SUITE_REGISTRATION(ServerIdentityVerifierTest);
