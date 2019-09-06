/*
 * Copyright (c) 2010-2016 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#include <memory>

#include <boost/bind.hpp>

#include <QA/Checker/IO.h>

#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/extensions/TestFactoryRegistry.h>

#include <Swiften/Base/ByteArray.h>
#include <Swiften/TLS/CertificateFactory.h>

#include <SwifTools/Application/PlatformApplicationPathProvider.h>

using namespace Swift;

template<typename CERTIFICATE_FACTORY>
class CertificateTest : public CppUnit::TestFixture {
        CPPUNIT_TEST_SUITE(CertificateTest);
        CPPUNIT_TEST(testConstructFromDER);
        CPPUNIT_TEST(testToDER);
        //CPPUNIT_TEST(testGetSubjectName);
        CPPUNIT_TEST(testGetCommonNames);
        CPPUNIT_TEST(testGetSRVNames);
        CPPUNIT_TEST(testGetDNSNames);
        CPPUNIT_TEST(testGetXMPPAddresses);
        CPPUNIT_TEST(testCreateCertificateChain);
        CPPUNIT_TEST_SUITE_END();

    public:
        void setUp() {
            pathProvider = std::make_unique<PlatformApplicationPathProvider>("FileReadBytestreamTest");
            readByteArrayFromFile(certificateData, (pathProvider->getExecutableDir() / "jabber_org.crt"));
            readByteArrayFromFile(chainData, (pathProvider->getExecutableDir() / "certificateChain.pem"));
            certificateFactory = std::unique_ptr<CertificateFactory>(new CERTIFICATE_FACTORY());
        }

        void testConstructFromDER() {
            Certificate::ref testling = Certificate::ref(certificateFactory->createCertificateFromDER(certificateData));

            CPPUNIT_ASSERT_EQUAL(std::string("*.jabber.org"), testling->getCommonNames()[0]);
        }

        void testToDER() {
            Certificate::ref testling = Certificate::ref(certificateFactory->createCertificateFromDER(certificateData));

            CPPUNIT_ASSERT_EQUAL(certificateData, testling->toDER());
        }

/*
        void testGetSubjectName() {
            Certificate::ref testling = Certificate::ref(certificateFactory->createCertificateFromDER(certificateData);

            CPPUNIT_ASSERT_EQUAL(std::string("/description=114072-VMk8pdi1aj5kTXxO/C=US/ST=Colorado/L=Denver/O=Peter Saint-Andre/OU=StartCom Trusted Certificate Member/CN=*.jabber.org/emailAddress=hostmaster@jabber.org"), testling->getSubjectName());
        }
        */

        void testGetCommonNames() {
            Certificate::ref testling = Certificate::ref(certificateFactory->createCertificateFromDER(certificateData));

            CPPUNIT_ASSERT_EQUAL(1, static_cast<int>(testling->getCommonNames().size()));
            CPPUNIT_ASSERT_EQUAL(std::string("*.jabber.org"), testling->getCommonNames()[0]);
        }

        void testGetSRVNames() {
            Certificate::ref testling = Certificate::ref(certificateFactory->createCertificateFromDER(certificateData));

            CPPUNIT_ASSERT_EQUAL(1, static_cast<int>(testling->getSRVNames().size()));
            CPPUNIT_ASSERT_EQUAL(std::string("*.jabber.org"), testling->getSRVNames()[0]);
        }

        void testGetDNSNames() {
            Certificate::ref testling = Certificate::ref(certificateFactory->createCertificateFromDER(certificateData));

            CPPUNIT_ASSERT_EQUAL(2, static_cast<int>(testling->getDNSNames().size()));
            CPPUNIT_ASSERT_EQUAL(std::string("*.jabber.org"), testling->getDNSNames()[0]);
            CPPUNIT_ASSERT_EQUAL(std::string("jabber.org"), testling->getDNSNames()[1]);
        }

        void testGetXMPPAddresses() {
            Certificate::ref testling = Certificate::ref(certificateFactory->createCertificateFromDER(certificateData));

            CPPUNIT_ASSERT_EQUAL(1, static_cast<int>(testling->getXMPPAddresses().size()));
            CPPUNIT_ASSERT_EQUAL(std::string("*.jabber.org"), testling->getXMPPAddresses()[0]);
        }

        void testCreateCertificateChain() {
            // The input chain contains a 2-certificate chain:
            // the first certificate has:
            // a subject of "O=messaging,CN=Mixer Messaging Configuration,CN=badger.isode.net"
            // an issuer of "O=messaging, CN=New Messaging CA"
            // the second certificate has:
            // a subject of "O=messaging, CN=New Messaging CA"
            // an issuer of "O=messaging, CN=New Messaging CA"
            // i.e. it is a self-signed certificate
            std::vector<std::shared_ptr<Certificate>> chain = certificateFactory->createCertificateChain(chainData);
            CPPUNIT_ASSERT_EQUAL(2,static_cast<int>(chain.size()));
            CPPUNIT_ASSERT_EQUAL(std::string("Mixer Messaging Configuration"), chain[0]->getCommonNames()[0]);
            CPPUNIT_ASSERT_EQUAL(std::string("badger.isode.net"), chain[0]->getCommonNames()[1]);
            CPPUNIT_ASSERT_EQUAL(std::string("New Messaging CA"), chain[1]->getCommonNames()[0]);
        }

    private:
        std::unique_ptr<PlatformApplicationPathProvider> pathProvider;
        ByteArray certificateData;
        ByteArray chainData;
        std::unique_ptr<CertificateFactory> certificateFactory;
};

#ifdef HAVE_OPENSSL
#include <Swiften/TLS/OpenSSL/OpenSSLCertificateFactory.h>
CPPUNIT_TEST_SUITE_REGISTRATION(CertificateTest<OpenSSLCertificateFactory>);
#endif
