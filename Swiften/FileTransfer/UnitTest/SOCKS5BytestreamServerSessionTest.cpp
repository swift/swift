/*
 * Copyright (c) 2010-2016 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#include <boost/bind.hpp>

#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/extensions/TestFactoryRegistry.h>

#include <Swiften/Base/ByteArray.h>
#include <Swiften/Base/Concat.h>
#include <Swiften/Base/StartStopper.h>
#include <Swiften/EventLoop/DummyEventLoop.h>
#include <Swiften/FileTransfer/ByteArrayReadBytestream.h>
#include <Swiften/FileTransfer/SOCKS5BytestreamRegistry.h>
#include <Swiften/FileTransfer/SOCKS5BytestreamServerSession.h>
#include <Swiften/Network/DummyConnection.h>

using namespace Swift;

class SOCKS5BytestreamServerSessionTest : public CppUnit::TestFixture {
        CPPUNIT_TEST_SUITE(SOCKS5BytestreamServerSessionTest);
        CPPUNIT_TEST(testAuthenticate);
        CPPUNIT_TEST(testAuthenticate_Chunked);
        CPPUNIT_TEST(testRequest);
        CPPUNIT_TEST(testRequest_UnknownBytestream);
        CPPUNIT_TEST(testReceiveData);
        CPPUNIT_TEST(testReceiveData_Chunked);
        CPPUNIT_TEST(testDataStreamPauseStopsSendingData);
        CPPUNIT_TEST(testDataStreamResumeAfterPauseSendsData);
        CPPUNIT_TEST_SUITE_END();

    public:
        void setUp() {
            receivedDataChunks = 0;
            eventLoop = new DummyEventLoop();
            bytestreams = new SOCKS5BytestreamRegistry();
            connection = std::make_shared<DummyConnection>(eventLoop);
            connection->onDataSent.connect(boost::bind(&SOCKS5BytestreamServerSessionTest::handleDataWritten, this, _1));
            stream1 = std::make_shared<ByteArrayReadBytestream>(createByteArray("abcdefg"));
            finished = false;
        }

        void tearDown() {
            connection.reset();
            delete bytestreams;
            delete eventLoop;
        }

        void testAuthenticate() {
            std::shared_ptr<SOCKS5BytestreamServerSession> testling(createSession());
            StartStopper<SOCKS5BytestreamServerSession> stopper(testling.get());

            receive(createSafeByteArray("\x05\x02\x01\x02"));

            CPPUNIT_ASSERT(createByteArray("\x05\x00", 2) == receivedData);
        }

        void testAuthenticate_Chunked() {
            std::shared_ptr<SOCKS5BytestreamServerSession> testling(createSession());
            StartStopper<SOCKS5BytestreamServerSession> stopper(testling.get());

            receive(createSafeByteArray("\x05\x02\x01"));

            CPPUNIT_ASSERT_EQUAL(0, static_cast<int>(receivedData.size()));
            receive(createSafeByteArray("\x01"));
            CPPUNIT_ASSERT(createByteArray("\x05\x00", 2) == receivedData);
        }

        void testRequest() {
            std::shared_ptr<SOCKS5BytestreamServerSession> testling(createSession());
            StartStopper<SOCKS5BytestreamServerSession> stopper(testling.get());
            bytestreams->setHasBytestream("abcdef", true);
            authenticate();

            ByteArray hostname(createByteArray("abcdef"));
            receive(concat(createSafeByteArray("\x05\x01\x00\x03", 4), createSafeByteArray(static_cast<char>(hostname.size())), createSafeByteArray(hostname), createSafeByteArray("\x00\x00", 2)));
            CPPUNIT_ASSERT(createByteArray("\x05\x00\x00\x03\x06\x61\x62\x63\x64\x65\x66\x00\x00", 13) == createByteArray(&receivedData[0], 13));
        }

        void testRequest_UnknownBytestream() {
            std::shared_ptr<SOCKS5BytestreamServerSession> testling(createSession());
            StartStopper<SOCKS5BytestreamServerSession> stopper(testling.get());
            authenticate();

            ByteArray hostname(createByteArray("abcdef"));
            receive(concat(createSafeByteArray("\x05\x01\x00\x03", 4), createSafeByteArray(static_cast<char>(hostname.size())), createSafeByteArray(hostname), createSafeByteArray("\x00\x00", 2)));
            CPPUNIT_ASSERT(createByteArray("\x05\x04\x00\x03\x06\x61\x62\x63\x64\x65\x66\x00\x00", 13) == receivedData);
        }

        void testReceiveData() {
            std::shared_ptr<SOCKS5BytestreamServerSession> testling(createSession());
            StartStopper<SOCKS5BytestreamServerSession> stopper(testling.get());
            bytestreams->setHasBytestream("abcdef", true);
            authenticate();
            request("abcdef");
            eventLoop->processEvents();
            testling->startSending(stream1);
            skipHeader("abcdef");
            eventLoop->processEvents();

            CPPUNIT_ASSERT(createByteArray("abcdefg") == receivedData);
            CPPUNIT_ASSERT_EQUAL(2, receivedDataChunks);
        }

        void testReceiveData_Chunked() {
            std::shared_ptr<SOCKS5BytestreamServerSession> testling(createSession());
            testling->setChunkSize(3);
            StartStopper<SOCKS5BytestreamServerSession> stopper(testling.get());
            bytestreams->setHasBytestream("abcdef", true);
            authenticate();
            request("abcdef");
            eventLoop->processEvents();
            testling->startSending(stream1);
            eventLoop->processEvents();
            skipHeader("abcdef");
            CPPUNIT_ASSERT(createByteArray("abcdefg") == receivedData);
            CPPUNIT_ASSERT_EQUAL(4, receivedDataChunks);
        }

        void testDataStreamPauseStopsSendingData() {
            std::shared_ptr<SOCKS5BytestreamServerSession> testling(createSession());
            testling->setChunkSize(3);
            stream1->setDataComplete(false);
            StartStopper<SOCKS5BytestreamServerSession> stopper(testling.get());
            bytestreams->setHasBytestream("abcdef", true);
            authenticate();
            request("abcdef");
            eventLoop->processEvents();
            testling->startSending(stream1);
            eventLoop->processEvents();
            skipHeader("abcdef");
            CPPUNIT_ASSERT(createByteArray("abcdefg") == receivedData);
            CPPUNIT_ASSERT_EQUAL(4, receivedDataChunks);

            CPPUNIT_ASSERT(!finished);
            CPPUNIT_ASSERT(!error);
        }

        void testDataStreamResumeAfterPauseSendsData() {
            std::shared_ptr<SOCKS5BytestreamServerSession> testling(createSession());
            testling->setChunkSize(3);
            stream1->setDataComplete(false);
            StartStopper<SOCKS5BytestreamServerSession> stopper(testling.get());
            bytestreams->setHasBytestream("abcdef", true);
            authenticate();
            request("abcdef");
            eventLoop->processEvents();
            testling->startSending(stream1);
            eventLoop->processEvents();
            skipHeader("abcdef");

            stream1->addData(createByteArray("xyz"));
            eventLoop->processEvents();

            CPPUNIT_ASSERT(createByteArray("abcdefgxyz") == receivedData);
            CPPUNIT_ASSERT(!finished);
            CPPUNIT_ASSERT(!error);
        }

    private:
        void receive(const SafeByteArray& data) {
            connection->receive(data);
            eventLoop->processEvents();
        }

        void authenticate() {
            receive(createSafeByteArray("\x05\x02\x01\x02"));
            receivedData.clear();
            receivedDataChunks = 0;
        }

        void request(const std::string& hostname) {
            receive(concat(createSafeByteArray("\x05\x01\x00\x03", 4), createSafeByteArray(static_cast<char>(hostname.size())), createSafeByteArray(hostname), createSafeByteArray("\x00\x00", 2)));
        }

        void skipHeader(const std::string& hostname) {
            size_t headerSize = 7 + hostname.size();
            receivedData = createByteArray(&receivedData[headerSize], receivedData.size() - headerSize);
        }


        void handleDataWritten(const SafeByteArray& data) {
            receivedData.insert(receivedData.end(), data.begin(), data.end());
            receivedDataChunks++;
        }

    private:
        SOCKS5BytestreamServerSession* createSession() {
            SOCKS5BytestreamServerSession* session = new SOCKS5BytestreamServerSession(connection, bytestreams);
            session->onFinished.connect(boost::bind(&SOCKS5BytestreamServerSessionTest::handleFinished, this, _1));
            return session;
        }

        void handleFinished(boost::optional<FileTransferError> error) {
            finished = true;
            this->error = error;
        }

    private:
        DummyEventLoop* eventLoop;
        SOCKS5BytestreamRegistry* bytestreams;
        std::shared_ptr<DummyConnection> connection;
        std::vector<unsigned char> receivedData;
        int receivedDataChunks;
        std::shared_ptr<ByteArrayReadBytestream> stream1;
        bool finished;
        boost::optional<FileTransferError> error;
};

CPPUNIT_TEST_SUITE_REGISTRATION(SOCKS5BytestreamServerSessionTest);
