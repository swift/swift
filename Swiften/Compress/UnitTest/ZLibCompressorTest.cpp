/*
 * Copyright (c) 2010 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#include <Swiften/Base/SafeByteArray.h>
#include <QA/Checker/IO.h>

#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/extensions/TestFactoryRegistry.h>

#include <Swiften/Compress/ZLibCompressor.h>

using namespace Swift;


class ZLibCompressorTest : public CppUnit::TestFixture
{
		CPPUNIT_TEST_SUITE(ZLibCompressorTest);
		CPPUNIT_TEST(testProcess);
		CPPUNIT_TEST(testProcess_Twice);
		CPPUNIT_TEST_SUITE_END();

	public:
		ZLibCompressorTest() {}

		void testProcess() {
			ZLibCompressor testling;
			SafeByteArray result = testling.process(createSafeByteArray("foo"));

			CPPUNIT_ASSERT_EQUAL(createSafeByteArray("\x78\xda\x4a\xcb\xcf\x07\x00\x00\x00\xff\xff", 11), result);
		}

		void testProcess_Twice() {
			ZLibCompressor testling;
			testling.process(createSafeByteArray("foo"));
			SafeByteArray result = testling.process(createSafeByteArray("bar"));

			CPPUNIT_ASSERT_EQUAL(createSafeByteArray("\x4a\x4a\x2c\x02\x00\x00\x00\xff\xff",9), result);
		}
};

CPPUNIT_TEST_SUITE_REGISTRATION(ZLibCompressorTest);
