/*
 * Copyright (c) 2010 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/extensions/TestFactoryRegistry.h>

#include <Swiften/Base/ByteArray.h>
#include <Swiften/FileTransfer/FileReadBytestream.h>
#include "SwifTools/Application/PlatformApplicationPathProvider.h"

using namespace Swift;

class FileReadBytestreamTest : public CppUnit::TestFixture {
		CPPUNIT_TEST_SUITE(FileReadBytestreamTest);
		CPPUNIT_TEST(testRead);
		CPPUNIT_TEST(testRead_Twice);
		CPPUNIT_TEST(testIsFinished_NotFinished);
		CPPUNIT_TEST(testIsFinished_IsFinished);
		CPPUNIT_TEST_SUITE_END();

	public:
		void setUp() {
			pathProvider = new PlatformApplicationPathProvider("FileReadBytestreamTest");
		}

		void tearDown() {
			delete pathProvider;
		}

		void testRead() {
			boost::shared_ptr<FileReadBytestream> testling(createTestling());

			std::vector<unsigned char> result = testling->read(10);

			CPPUNIT_ASSERT(ByteArray::create("/*\n * Copy") == result);
		}

		void testRead_Twice() {
			boost::shared_ptr<FileReadBytestream> testling(createTestling());

			testling->read(10);
			ByteArray result(testling->read(10));

			CPPUNIT_ASSERT_EQUAL(std::string("right (c) "), result.toString());
		}

		void testIsFinished_NotFinished() {
			boost::shared_ptr<FileReadBytestream> testling(createTestling());

			testling->read(10);

			CPPUNIT_ASSERT(!testling->isFinished());
		}

		void testIsFinished_IsFinished() {
			boost::shared_ptr<FileReadBytestream> testling(createTestling());

			testling->read(4096);

			CPPUNIT_ASSERT(testling->isFinished());
		}

	private:
		FileReadBytestream* createTestling() {
			return new FileReadBytestream(pathProvider->getExecutableDir() / "FileReadBytestreamTest.cpp");
		}

		PlatformApplicationPathProvider* pathProvider;
};

CPPUNIT_TEST_SUITE_REGISTRATION(FileReadBytestreamTest);
