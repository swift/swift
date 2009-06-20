#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/extensions/TestFactoryRegistry.h>

#include "Swiften/StringCodecs/SHA1.h"

using namespace Swift;

class SHA1Test : public CppUnit::TestFixture
{
		CPPUNIT_TEST_SUITE(SHA1Test);
		CPPUNIT_TEST(testGetBinaryHash);
		CPPUNIT_TEST(testGetHexHash);
		CPPUNIT_TEST_SUITE_END();

	public:
		SHA1Test() {}

		void testGetBinaryHash() {
			ByteArray result(SHA1::getBinaryHash("client/pc//Exodus 0.9.1<http://jabber.org/protocol/caps<http://jabber.org/protocol/disco#info<http://jabber.org/protocol/disco#items<http://jabber.org/protocol/muc<"));
			CPPUNIT_ASSERT_EQUAL(ByteArray("\x42\x06\xb2\x3c\xa6\xb0\xa6\x43\xd2\x0d\x89\xb0\x4f\xf5\x8c\xf7\x8b\x80\x96\xed"), result);
		}

		void testGetHexHash() {
			String result(SHA1::getHexHash("client/pc//Exodus 0.9.1<http://jabber.org/protocol/caps<http://jabber.org/protocol/disco#info<http://jabber.org/protocol/disco#items<http://jabber.org/protocol/muc<"));
			CPPUNIT_ASSERT_EQUAL(String("4206b23ca6b0a643d20d89b04ff58cf78b8096ed"), result);
		}
};

CPPUNIT_TEST_SUITE_REGISTRATION(SHA1Test);
