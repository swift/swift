/*
 * Copyright (c) 2010 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/extensions/TestFactoryRegistry.h>

#include <Swiften/StringCodecs/Hexify.h>
#include <string>
#include <Swiften/Base/ByteArray.h>

using namespace Swift;

class HexifyTest : public CppUnit::TestFixture {
		CPPUNIT_TEST_SUITE(HexifyTest);
		CPPUNIT_TEST(testHexify);
		CPPUNIT_TEST(testHexify_Byte);
		CPPUNIT_TEST(testUnhexify);
		CPPUNIT_TEST_SUITE_END();

	public:
		void testHexify() {
			CPPUNIT_ASSERT_EQUAL(std::string("4206b23ca6b0a643d20d89b04ff58cf78b8096ed"), Hexify::hexify(createByteArray("\x42\x06\xb2\x3c\xa6\xb0\xa6\x43\xd2\x0d\x89\xb0\x4f\xf5\x8c\xf7\x8b\x80\x96\xed")));
		}

		void testHexify_Byte() {
			CPPUNIT_ASSERT_EQUAL(std::string("b2"), Hexify::hexify(0xb2));
		}

		void testUnhexify() {
			CPPUNIT_ASSERT_EQUAL(std::string("ffaf02"), Hexify::hexify(Hexify::unhexify("ffaf02")));
			CPPUNIT_ASSERT(createByteArray("\x01\x23\xf2", 3) == Hexify::unhexify("0123f2"));
		}
};

CPPUNIT_TEST_SUITE_REGISTRATION(HexifyTest);
