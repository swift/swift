/*
 * Copyright (c) 2010 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/extensions/TestFactoryRegistry.h>
#include <boost/bind.hpp>

#include "Swiften/TLS/OpenSSL/OpenSSLCertificate.h"
#include "SwifTools/Application/PlatformApplicationPathProvider.h"

using namespace Swift;

class OpenSSLCertificateTest : public CppUnit::TestFixture {
		CPPUNIT_TEST_SUITE(OpenSSLCertificateTest);
		CPPUNIT_TEST(testConstructFromDER);
		CPPUNIT_TEST(testToDER);
		CPPUNIT_TEST(testGetSubjectName);
		CPPUNIT_TEST_SUITE_END();

	public:
		void setUp() {
			pathProvider = new PlatformApplicationPathProvider("FileReadBytestreamTest");
			certificateData.readFromFile((pathProvider->getExecutableDir() / "jabber_org.crt").string());
		}

		void tearDown() {
			delete pathProvider;
		}

		void testConstructFromDER() {
			OpenSSLCertificate testling(certificateData);

			CPPUNIT_ASSERT_EQUAL(String("*.jabber.org"), testling.getCommonNames()[0]);
		}
		
		void testToDER() {
			OpenSSLCertificate testling(certificateData);

			CPPUNIT_ASSERT_EQUAL(certificateData, testling.toDER());
		}

		void testGetSubjectName() {
			OpenSSLCertificate testling(certificateData);

			CPPUNIT_ASSERT_EQUAL(String("/description=114072-VMk8pdi1aj5kTXxO/C=US/ST=Colorado/L=Denver/O=Peter Saint-Andre/OU=StartCom Trusted Certificate Member/CN=*.jabber.org/emailAddress=hostmaster@jabber.org"), testling.getSubjectName());
		}
	
	private:
		PlatformApplicationPathProvider* pathProvider;
		ByteArray certificateData;
};

CPPUNIT_TEST_SUITE_REGISTRATION(OpenSSLCertificateTest);
