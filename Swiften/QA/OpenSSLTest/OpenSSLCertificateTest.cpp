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
		CPPUNIT_TEST_SUITE_END();

	public:
		void setUp() {
			pathProvider = new PlatformApplicationPathProvider("FileReadBytestreamTest");
		}

		void tearDown() {
			delete pathProvider;
		}

		void testConstructFromDER() {
			ByteArray in;
			in.readFromFile((pathProvider->getExecutableDir() / "jabber_org.crt").string());
			OpenSSLCertificate testling(in);

			CPPUNIT_ASSERT_EQUAL(String("*.jabber.org"), testling.getCommonName());
		}
		
		void testToDER() {
			ByteArray in;
			in.readFromFile((pathProvider->getExecutableDir() / "jabber_org.crt").string());
			OpenSSLCertificate testling(in);

			CPPUNIT_ASSERT_EQUAL(in, testling.toDER());
		}
	
	private:
		PlatformApplicationPathProvider* pathProvider;
};

CPPUNIT_TEST_SUITE_REGISTRATION(OpenSSLCertificateTest);
