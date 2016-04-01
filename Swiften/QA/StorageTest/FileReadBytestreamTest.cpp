/*
 * Copyright (c) 2010-2016 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/extensions/TestFactoryRegistry.h>

#include <Swiften/Base/ByteArray.h>
#include <Swiften/FileTransfer/FileReadBytestream.h>

#include <SwifTools/Application/PlatformApplicationPathProvider.h>

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
            std::shared_ptr<FileReadBytestream> testling(createTestling());

            std::shared_ptr< std::vector<unsigned char> > result = testling->read(10);

            CPPUNIT_ASSERT(createByteArray("/*\n * Copy") == *result.get());
        }

        void testRead_Twice() {
            std::shared_ptr<FileReadBytestream> testling(createTestling());

            testling->read(10);
            std::shared_ptr< std::vector<unsigned char> > result = testling->read(10);

            CPPUNIT_ASSERT_EQUAL(std::string("right (c) "), byteArrayToString(*result));
        }

        void testIsFinished_NotFinished() {
            std::shared_ptr<FileReadBytestream> testling(createTestling());

            testling->read(10);

            CPPUNIT_ASSERT(!testling->isFinished());
        }

        void testIsFinished_IsFinished() {
            std::shared_ptr<FileReadBytestream> testling(createTestling());

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
