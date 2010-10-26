/*
 * Copyright (c) 2010 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#include "Swiften/Base/ByteArray.h"

#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/extensions/TestFactoryRegistry.h>

#include "Swiften/StringCodecs/SHA1.h"

using namespace Swift;

class SHA1Test : public CppUnit::TestFixture {
		CPPUNIT_TEST_SUITE(SHA1Test);
		CPPUNIT_TEST(testGetHash);
		CPPUNIT_TEST(testGetHash_Twice);
		CPPUNIT_TEST(testGetHash_NoData);
		CPPUNIT_TEST_SUITE_END();

	public:
		void testGetHash() {
			ByteArray result(SHA1::getHash("client/pc//Exodus 0.9.1<http://jabber.org/protocol/caps<http://jabber.org/protocol/disco#info<http://jabber.org/protocol/disco#items<http://jabber.org/protocol/muc<"));
			CPPUNIT_ASSERT_EQUAL(ByteArray("\x42\x06\xb2\x3c\xa6\xb0\xa6\x43\xd2\x0d\x89\xb0\x4f\xf5\x8c\xf7\x8b\x80\x96\xed"), result);
		}


		void testGetHash_Twice() {
			ByteArray input("client/pc//Exodus 0.9.1<http://jabber.org/protocol/caps<http://jabber.org/protocol/disco#info<http://jabber.org/protocol/disco#items<http://jabber.org/protocol/muc<");
			SHA1::getHash(input);
			ByteArray result(SHA1::getHash(input));

			CPPUNIT_ASSERT_EQUAL(ByteArray("\x42\x06\xb2\x3c\xa6\xb0\xa6\x43\xd2\x0d\x89\xb0\x4f\xf5\x8c\xf7\x8b\x80\x96\xed"), result);
		}

		void testGetHash_NoData() {
			ByteArray result(SHA1::getHash(ByteArray()));

			CPPUNIT_ASSERT_EQUAL(ByteArray("\xda\x39\xa3\xee\x5e\x6b\x4b\x0d\x32\x55\xbf\xef\x95\x60\x18\x90\xaf\xd8\x07\x09"), result);
		}
};

CPPUNIT_TEST_SUITE_REGISTRATION(SHA1Test);
