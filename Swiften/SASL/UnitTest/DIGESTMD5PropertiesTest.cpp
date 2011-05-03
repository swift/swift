/*
 * Copyright (c) 2010 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/extensions/TestFactoryRegistry.h>

#include <Swiften/SASL/DIGESTMD5Properties.h>

using namespace Swift;

class DIGESTMD5PropertiesTest : public CppUnit::TestFixture {
		CPPUNIT_TEST_SUITE(DIGESTMD5PropertiesTest);
		CPPUNIT_TEST(testParse);
		CPPUNIT_TEST(testSerialize);
		CPPUNIT_TEST_SUITE_END();

	public:
		void testParse() {
			DIGESTMD5Properties properties = DIGESTMD5Properties::parse(createByteArray(
					"realm=\"myrealm1\",realm=\"myrealm2\",nonce=\"mynonce\","
					"algorithm=md5-sess,charset=utf-8"));

			CPPUNIT_ASSERT(properties.getValue("realm"));
			CPPUNIT_ASSERT_EQUAL(std::string("myrealm1"), *properties.getValue("realm"));
			CPPUNIT_ASSERT(properties.getValue("nonce"));
			CPPUNIT_ASSERT_EQUAL(std::string("mynonce"), *properties.getValue("nonce"));
			CPPUNIT_ASSERT(properties.getValue("algorithm"));
			CPPUNIT_ASSERT_EQUAL(std::string("md5-sess"), *properties.getValue("algorithm"));
			CPPUNIT_ASSERT(properties.getValue("charset"));
			CPPUNIT_ASSERT_EQUAL(std::string("utf-8"), *properties.getValue("charset"));
		}

		void testSerialize() {
			DIGESTMD5Properties properties;
			properties.setValue("authzid", "myauthzid");
			properties.setValue("charset", "utf-8");
			properties.setValue("cnonce", "mycnonce");
			properties.setValue("digest-uri", "mydigesturi");
			properties.setValue("nc", "1");
			properties.setValue("nonce", "mynonce");
			properties.setValue("qop", "auth");
			properties.setValue("realm", "myrealm");
			properties.setValue("response", "myresponse");
			properties.setValue("username", "myuser");

			ByteArray result = properties.serialize();
			ByteArray expected(createByteArray("authzid=\"myauthzid\",charset=utf-8,cnonce=\"mycnonce\",digest-uri=\"mydigesturi\",nc=1,nonce=\"mynonce\",qop=auth,realm=\"myrealm\",response=myresponse,username=\"myuser\""));
			CPPUNIT_ASSERT_EQUAL(byteArrayToString(expected), byteArrayToString(result));
		}
};

CPPUNIT_TEST_SUITE_REGISTRATION(DIGESTMD5PropertiesTest);
