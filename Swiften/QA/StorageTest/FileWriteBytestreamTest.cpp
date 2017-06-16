/*
 * Copyright (c) 2016 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#include <boost/bind.hpp>
#include <boost/filesystem.hpp>
#include <boost/signals2.hpp>

#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/extensions/TestFactoryRegistry.h>

#include <Swiften/Base/ByteArray.h>
#include <Swiften/FileTransfer/FileWriteBytestream.h>

using namespace Swift;

class FileWriteBytestreamTest : public CppUnit::TestFixture {
        CPPUNIT_TEST_SUITE(FileWriteBytestreamTest);
        CPPUNIT_TEST(testSuccessfulWrite);
        CPPUNIT_TEST(testFailingWrite);
        CPPUNIT_TEST_SUITE_END();

    public:
        void setUp() {
            onWriteWasCalled = false;
        }

        void testSuccessfulWrite() {
            boost::filesystem::path filename = boost::filesystem::unique_path("write_file_bytestream_test_%%%%%%%%%%%%%%%%.bin");
            std::shared_ptr<WriteBytestream> writeBytestream = std::make_shared<FileWriteBytestream>(filename.string());
            writeBytestream->onWrite.connect(boost::bind(&FileWriteBytestreamTest::handleOnWrite, this, _1));

            CPPUNIT_ASSERT_EQUAL(true, writeBytestream->write(createByteArray("Some data.")));
            CPPUNIT_ASSERT_EQUAL(true, onWriteWasCalled);
            writeBytestream->close();
            boost::filesystem::remove(filename);
        }

        void testFailingWrite() {
            std::shared_ptr<WriteBytestream> writeBytestream = std::make_shared<FileWriteBytestream>("");
            writeBytestream->onWrite.connect(boost::bind(&FileWriteBytestreamTest::handleOnWrite, this, _1));

            CPPUNIT_ASSERT_EQUAL(false, writeBytestream->write(createByteArray("Some data.")));
            CPPUNIT_ASSERT_EQUAL(false, onWriteWasCalled);
        }


        void handleOnWrite(const std::vector<unsigned char>& /*data*/) {
            onWriteWasCalled = true;
        }

    private:
        bool onWriteWasCalled;
};

CPPUNIT_TEST_SUITE_REGISTRATION(FileWriteBytestreamTest);
