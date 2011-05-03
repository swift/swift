/*
 * Copyright (c) 2010 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#include <Swiften/Base/ByteArray.h>

#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/extensions/TestFactoryRegistry.h>
#include <boost/bind.hpp>

#include <QA/Checker/IO.h>
#include <Swiften/TLS/CertificateFactory.h>
#include "SwifTools/Application/PlatformApplicationPathProvider.h"

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
		CPPUNIT_TEST_SUITE_END();

	public:
		void setUp() {
			pathProvider = new PlatformApplicationPathProvider("FileReadBytestreamTest");
			readByteArrayFromFile(certificateData, (pathProvider->getExecutableDir() / "jabber_org.crt").string());
			certificateFactory = new CERTIFICATE_FACTORY();
		}

		void tearDown() {
			delete certificateFactory;
			delete pathProvider;
		}

		void testConstructFromDER() {
			Certificate::ref testling = certificateFactory->createCertificateFromDER(certificateData);

			CPPUNIT_ASSERT_EQUAL(std::string("*.jabber.org"), testling->getCommonNames()[0]);
		}
		
		void testToDER() {
			Certificate::ref testling = certificateFactory->createCertificateFromDER(certificateData);

			CPPUNIT_ASSERT_EQUAL(certificateData, testling->toDER());
		}

/*
		void testGetSubjectName() {
			Certificate::ref testling = certificateFactory->createCertificateFromDER(certificateData);

			CPPUNIT_ASSERT_EQUAL(std::string("/description=114072-VMk8pdi1aj5kTXxO/C=US/ST=Colorado/L=Denver/O=Peter Saint-Andre/OU=StartCom Trusted Certificate Member/CN=*.jabber.org/emailAddress=hostmaster@jabber.org"), testling->getSubjectName());
		}
		*/

		void testGetCommonNames() {
			Certificate::ref testling = certificateFactory->createCertificateFromDER(certificateData);

			CPPUNIT_ASSERT_EQUAL(1, static_cast<int>(testling->getCommonNames().size()));
			CPPUNIT_ASSERT_EQUAL(std::string("*.jabber.org"), testling->getCommonNames()[0]);
		}

		void testGetSRVNames() {
			Certificate::ref testling = certificateFactory->createCertificateFromDER(certificateData);

			CPPUNIT_ASSERT_EQUAL(1, static_cast<int>(testling->getSRVNames().size()));
			CPPUNIT_ASSERT_EQUAL(std::string("*.jabber.org"), testling->getSRVNames()[0]);
		}

		void testGetDNSNames() {
			Certificate::ref testling = certificateFactory->createCertificateFromDER(certificateData);

			CPPUNIT_ASSERT_EQUAL(2, static_cast<int>(testling->getDNSNames().size()));
			CPPUNIT_ASSERT_EQUAL(std::string("*.jabber.org"), testling->getDNSNames()[0]);
			CPPUNIT_ASSERT_EQUAL(std::string("jabber.org"), testling->getDNSNames()[1]);
		}

		void testGetXMPPAddresses() {
			Certificate::ref testling = certificateFactory->createCertificateFromDER(certificateData);

			CPPUNIT_ASSERT_EQUAL(1, static_cast<int>(testling->getXMPPAddresses().size()));
			CPPUNIT_ASSERT_EQUAL(std::string("*.jabber.org"), testling->getXMPPAddresses()[0]);
		}
	
	private:
		PlatformApplicationPathProvider* pathProvider;
		ByteArray certificateData;
		CertificateFactory* certificateFactory;
};

#ifdef HAVE_OPENSSL
#include <Swiften/TLS/OpenSSL/OpenSSLCertificateFactory.h>
CPPUNIT_TEST_SUITE_REGISTRATION(CertificateTest<OpenSSLCertificateFactory>);
#endif
