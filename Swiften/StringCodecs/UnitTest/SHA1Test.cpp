/*
 * Copyright (c) 2010 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#include <Swiften/Base/ByteArray.h>
#include <QA/Checker/IO.h>

#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/extensions/TestFactoryRegistry.h>

#include <Swiften/StringCodecs/SHA1.h>

using namespace Swift;

class SHA1Test : public CppUnit::TestFixture {
		CPPUNIT_TEST_SUITE(SHA1Test);
		CPPUNIT_TEST(testGetHash);
		CPPUNIT_TEST(testGetHash_TwoUpdates);
		//CPPUNIT_TEST(testGetHash_TwoGetHash);
		CPPUNIT_TEST(testGetHash_NoData);
		//CPPUNIT_TEST(testGetHash_InterleavedUpdate);
		CPPUNIT_TEST(testGetHashStatic);
		CPPUNIT_TEST(testGetHashStatic_Twice);
		CPPUNIT_TEST(testGetHashStatic_NoData);
		CPPUNIT_TEST_SUITE_END();

	public:
		void testGetHash() {
			SHA1 sha;
			sha.update(createByteArray("client/pc//Exodus 0.9.1<http://jabber.org/protocol/caps<http://jabber.org/protocol/disco#info<http://jabber.org/protocol/disco#items<http://jabber.org/protocol/muc<"));

			CPPUNIT_ASSERT_EQUAL(createByteArray("\x42\x06\xb2\x3c\xa6\xb0\xa6\x43\xd2\x0d\x89\xb0\x4f\xf5\x8c\xf7\x8b\x80\x96\xed"), sha.getHash());
		}

		void testGetHash_TwoUpdates() {
			SHA1 sha;
			sha.update(createByteArray("client/pc//Exodus 0.9.1<http://jabber.org/protocol/caps<"));
			sha.update(createByteArray("http://jabber.org/protocol/disco#info<http://jabber.org/protocol/disco#items<http://jabber.org/protocol/muc<"));

			CPPUNIT_ASSERT_EQUAL(createByteArray("\x42\x06\xb2\x3c\xa6\xb0\xa6\x43\xd2\x0d\x89\xb0\x4f\xf5\x8c\xf7\x8b\x80\x96\xed"), sha.getHash());
		}

		void testGetHash_TwoGetHash() {
			SHA1 sha;
			sha.update(createByteArray("client/pc//Exodus 0.9.1<http://jabber.org/protocol/caps<http://jabber.org/protocol/disco#info<http://jabber.org/protocol/disco#items<http://jabber.org/protocol/muc<"));

			sha.getHash();

			CPPUNIT_ASSERT_EQUAL(createByteArray("\x42\x06\xb2\x3c\xa6\xb0\xa6\x43\xd2\x0d\x89\xb0\x4f\xf5\x8c\xf7\x8b\x80\x96\xed"), sha.getHash());
		}

		void testGetHash_InterleavedUpdate() {
			SHA1 sha;

			sha.update(createByteArray("client/pc//Exodus 0.9.1<http://jabber.org/protocol/caps<"));
			sha.getHash();
			sha.update(createByteArray("http://jabber.org/protocol/disco#info<http://jabber.org/protocol/disco#items<http://jabber.org/protocol/muc<"));

			CPPUNIT_ASSERT_EQUAL(createByteArray("\x42\x06\xb2\x3c\xa6\xb0\xa6\x43\xd2\x0d\x89\xb0\x4f\xf5\x8c\xf7\x8b\x80\x96\xed"), sha.getHash());
		}


		void testGetHash_NoData() {
			SHA1 sha;
			sha.update(std::vector<unsigned char>());

			CPPUNIT_ASSERT_EQUAL(createByteArray("\xda\x39\xa3\xee\x5e\x6b\x4b\x0d\x32\x55\xbf\xef\x95\x60\x18\x90\xaf\xd8\x07\x09"), sha.getHash());
		}

		void testGetHashStatic() {
			ByteArray result(SHA1::getHash(createByteArray("client/pc//Exodus 0.9.1<http://jabber.org/protocol/caps<http://jabber.org/protocol/disco#info<http://jabber.org/protocol/disco#items<http://jabber.org/protocol/muc<")));
			CPPUNIT_ASSERT_EQUAL(createByteArray("\x42\x06\xb2\x3c\xa6\xb0\xa6\x43\xd2\x0d\x89\xb0\x4f\xf5\x8c\xf7\x8b\x80\x96\xed"), result);
		}


		void testGetHashStatic_Twice() {
			ByteArray input(createByteArray("client/pc//Exodus 0.9.1<http://jabber.org/protocol/caps<http://jabber.org/protocol/disco#info<http://jabber.org/protocol/disco#items<http://jabber.org/protocol/muc<"));
			SHA1::getHash(input);
			ByteArray result(SHA1::getHash(input));

			CPPUNIT_ASSERT_EQUAL(createByteArray("\x42\x06\xb2\x3c\xa6\xb0\xa6\x43\xd2\x0d\x89\xb0\x4f\xf5\x8c\xf7\x8b\x80\x96\xed"), result);
		}

		void testGetHashStatic_NoData() {
			ByteArray result(SHA1::getHash(ByteArray()));

			CPPUNIT_ASSERT_EQUAL(createByteArray("\xda\x39\xa3\xee\x5e\x6b\x4b\x0d\x32\x55\xbf\xef\x95\x60\x18\x90\xaf\xd8\x07\x09"), result);
		}
};

CPPUNIT_TEST_SUITE_REGISTRATION(SHA1Test);
