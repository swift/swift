/*
 * Copyright (c) 2010 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#include "Swiften/Base/ByteArray.h"

#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/extensions/TestFactoryRegistry.h>

#include "Swiften/Compress/ZLibDecompressor.h"
#include "Swiften/Compress/ZLibCompressor.h"
#include "Swiften/Compress/ZLibException.h"

using namespace Swift;


class ZLibDecompressorTest : public CppUnit::TestFixture
{
		CPPUNIT_TEST_SUITE(ZLibDecompressorTest);
		CPPUNIT_TEST(testProcess);
		CPPUNIT_TEST(testProcess_Twice);
		CPPUNIT_TEST(testProcess_Invalid);
		CPPUNIT_TEST(testProcess_Huge);
		CPPUNIT_TEST(testProcess_ChunkSize);
		CPPUNIT_TEST_SUITE_END();

	public:
		ZLibDecompressorTest() {}

		void testProcess() {
			ZLibDecompressor testling;
			ByteArray result = testling.process(ByteArray("\x78\xda\x4a\xcb\xcf\x07\x00\x00\x00\xff\xff", 11));

			CPPUNIT_ASSERT_EQUAL(ByteArray("foo"), result);
		}

		void testProcess_Twice() {
			ZLibDecompressor testling;
			testling.process(ByteArray("\x78\xda\x4a\xcb\xcf\x07\x00\x00\x00\xff\xff", 11));
			ByteArray result = testling.process(ByteArray("\x4a\x4a\x2c\x02\x00\x00\x00\xff\xff", 9)); 

			CPPUNIT_ASSERT_EQUAL(ByteArray("bar"), result);
		}

		void testProcess_Invalid() {
			ZLibDecompressor testling;
			CPPUNIT_ASSERT_THROW(testling.process(ByteArray("invalid")), ZLibException);
		}

		void testProcess_Huge() {
			std::vector<char> data;
			data.reserve(2048);
			for (unsigned int i = 0; i < 2048; ++i) {
				data.push_back(static_cast<char>(i));
			}
			ByteArray original(&data[0], data.size());
			ByteArray compressed = ZLibCompressor().process(original);
			ByteArray decompressed = ZLibDecompressor().process(compressed);

			CPPUNIT_ASSERT_EQUAL(original, decompressed);
		}

		void testProcess_ChunkSize() {
			std::vector<char> data;
			data.reserve(1024);
			for (unsigned int i = 0; i < 1024; ++i) {
				data.push_back(static_cast<char>(i));
			}
			ByteArray original(&data[0], data.size());
			ByteArray compressed = ZLibCompressor().process(original);
			ByteArray decompressed = ZLibDecompressor().process(compressed);

			CPPUNIT_ASSERT_EQUAL(original, decompressed);
		}
};

CPPUNIT_TEST_SUITE_REGISTRATION(ZLibDecompressorTest);
