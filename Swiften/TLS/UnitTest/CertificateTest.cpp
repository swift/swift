/*
 * Copyright (c) 2010 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#include <Swiften/Base/ByteArray.h>

#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/extensions/TestFactoryRegistry.h>
#include <boost/smart_ptr/make_shared.hpp>

#include <Swiften/TLS/Certificate.h>
#include <Swiften/TLS/SimpleCertificate.h>

using namespace Swift;

class CertificateTest : public CppUnit::TestFixture {
		CPPUNIT_TEST_SUITE(CertificateTest);
		CPPUNIT_TEST(testGetSHA1Fingerprint);
		CPPUNIT_TEST_SUITE_END();

	public:
		void testGetSHA1Fingerprint() {
			SimpleCertificate::ref testling = boost::make_shared<SimpleCertificate>();
			testling->setDER(createByteArray("abcdefg"));

			CPPUNIT_ASSERT_EQUAL(std::string("2f:b5:e1:34:19:fc:89:24:68:65:e7:a3:24:f4:76:ec:62:4e:87:40"), testling->getSHA1Fingerprint());
		}
};

CPPUNIT_TEST_SUITE_REGISTRATION(CertificateTest);
