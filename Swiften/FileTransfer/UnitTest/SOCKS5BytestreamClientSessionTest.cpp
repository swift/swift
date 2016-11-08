/*
 * Copyright (c) 2011 Tobias Markmann
 * Licensed under the simplified BSD license.
 * See Documentation/Licenses/BSD-simplified.txt for more information.
 */

/*
 * Copyright (c) 2015-2016 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#include <memory>

#include <boost/bind.hpp>
#include <boost/random/mersenne_twister.hpp>
#include <boost/random/uniform_int.hpp>
#include <boost/random/variate_generator.hpp>

#include <QA/Checker/IO.h>

#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/extensions/TestFactoryRegistry.h>

#include <Swiften/Base/Algorithm.h>
#include <Swiften/Base/ByteArray.h>
#include <Swiften/Base/Concat.h>
#include <Swiften/Base/Log.h>
#include <Swiften/Base/StartStopper.h>
#include <Swiften/Crypto/CryptoProvider.h>
#include <Swiften/Crypto/PlatformCryptoProvider.h>
#include <Swiften/EventLoop/DummyEventLoop.h>
#include <Swiften/FileTransfer/ByteArrayReadBytestream.h>
#include <Swiften/FileTransfer/ByteArrayWriteBytestream.h>
#include <Swiften/FileTransfer/SOCKS5BytestreamClientSession.h>
#include <Swiften/FileTransfer/SOCKS5BytestreamRegistry.h>
#include <Swiften/JID/JID.h>
#include <Swiften/Network/DummyConnection.h>
#include <Swiften/Network/DummyTimerFactory.h>
#include <Swiften/StringCodecs/Hexify.h>

using namespace Swift;

static boost::mt19937 randomGen;

class SOCKS5BytestreamClientSessionTest : public CppUnit::TestFixture {
    CPPUNIT_TEST_SUITE(SOCKS5BytestreamClientSessionTest);
    CPPUNIT_TEST(testForSessionReady);
    CPPUNIT_TEST(testErrorHandlingHello);
    CPPUNIT_TEST(testErrorHandlingRequest);
    CPPUNIT_TEST(testWriteBytestream);
    CPPUNIT_TEST(testReadBytestream);
    CPPUNIT_TEST_SUITE_END();

public:
    SOCKS5BytestreamClientSessionTest() : destinationAddressPort(HostAddressPort(HostAddress::fromString("127.0.0.1").get(), 8888)) {}

    void setUp() {
        crypto = std::shared_ptr<CryptoProvider>(PlatformCryptoProvider::create());
        destination = "092a44d859d19c9eed676b551ee80025903351c2";
        randomGen.seed(static_cast<unsigned int>(time(nullptr)));
        eventLoop = std::unique_ptr<DummyEventLoop>(new DummyEventLoop());
        timerFactory = std::unique_ptr<DummyTimerFactory>(new DummyTimerFactory());
        connection = std::make_shared<MockeryConnection>(failingPorts, true, eventLoop.get());
        //connection->onDataSent.connect(boost::bind(&SOCKS5BytestreamServerSessionTest::handleDataWritten, this, _1));
        //stream1 = std::make_shared<ByteArrayReadBytestream>(createByteArray("abcdefg")));
//        connection->onDataRead.connect(boost::bind(&SOCKS5BytestreamClientSessionTest::handleDataRead, this, _1));
    }

    void tearDown() {
        //connection.reset();
    }

    void testForSessionReady() {
        TestHelper helper;
        connection->onDataSent.connect(boost::bind(&TestHelper::handleConnectionDataWritten, &helper, _1));

        SOCKS5BytestreamClientSession::ref clientSession = std::make_shared<SOCKS5BytestreamClientSession>(connection, destinationAddressPort, destination, timerFactory.get());
        clientSession->onSessionReady.connect(boost::bind(&TestHelper::handleSessionReady, &helper, _1));

        clientSession->start();
        eventLoop->processEvents();
        CPPUNIT_ASSERT(createByteArray("\x05\x01\x00", 3) == helper.unprocessedInput);

        helper.unprocessedInput.clear();
        serverRespondHelloOK();
        eventLoop->processEvents();
        CPPUNIT_ASSERT_EQUAL(createByteArray("\x05\x01\x00\x03", 4), createByteArray(&helper.unprocessedInput[0], 4));
        CPPUNIT_ASSERT_EQUAL(createByteArray(static_cast<char>(destination.size())), createByteArray(static_cast<char>(helper.unprocessedInput[4])));
        CPPUNIT_ASSERT_EQUAL(createByteArray(destination), createByteArray(&helper.unprocessedInput[5], destination.size()));
        CPPUNIT_ASSERT_EQUAL(createByteArray("\x00", 1), createByteArray(&helper.unprocessedInput[5 + destination.size()], 1));

        helper.unprocessedInput.clear();
        serverRespondRequestOK();
        eventLoop->processEvents();
        CPPUNIT_ASSERT_EQUAL(true, helper.sessionReadyCalled);
        CPPUNIT_ASSERT_EQUAL(false, helper.sessionReadyError);
    }

    void testErrorHandlingHello() {
        TestHelper helper;
        connection->onDataSent.connect(boost::bind(&TestHelper::handleConnectionDataWritten, &helper, _1));

        SOCKS5BytestreamClientSession::ref clientSession = std::make_shared<SOCKS5BytestreamClientSession>(connection, destinationAddressPort, destination, timerFactory.get());
        clientSession->onSessionReady.connect(boost::bind(&TestHelper::handleSessionReady, &helper, _1));

        clientSession->start();
        eventLoop->processEvents();
        CPPUNIT_ASSERT_EQUAL(createByteArray("\x05\x01\x00", 3), helper.unprocessedInput);

        helper.unprocessedInput.clear();
        serverRespondHelloAuthFail();
        eventLoop->processEvents();

        CPPUNIT_ASSERT_EQUAL(true, helper.sessionReadyCalled);
        CPPUNIT_ASSERT_EQUAL(true, helper.sessionReadyError);
        CPPUNIT_ASSERT_EQUAL(true, connection->disconnectCalled);
    }

    void testErrorHandlingRequest() {
        TestHelper helper;
        connection->onDataSent.connect(boost::bind(&TestHelper::handleConnectionDataWritten, &helper, _1));

        SOCKS5BytestreamClientSession::ref clientSession = std::make_shared<SOCKS5BytestreamClientSession>(connection, destinationAddressPort, destination, timerFactory.get());
        clientSession->onSessionReady.connect(boost::bind(&TestHelper::handleSessionReady, &helper, _1));

        clientSession->start();
        eventLoop->processEvents();
        CPPUNIT_ASSERT_EQUAL(createByteArray("\x05\x01\x00", 3), helper.unprocessedInput);

        helper.unprocessedInput.clear();
        serverRespondHelloOK();
        eventLoop->processEvents();
        CPPUNIT_ASSERT_EQUAL(createByteArray("\x05\x01\x00\x03", 4), createByteArray(&helper.unprocessedInput[0], 4));
        CPPUNIT_ASSERT_EQUAL(createByteArray(static_cast<char>(destination.size())), createByteArray(static_cast<char>(helper.unprocessedInput[4])));
        CPPUNIT_ASSERT_EQUAL(createByteArray(destination), createByteArray(&helper.unprocessedInput[5], destination.size()));
        CPPUNIT_ASSERT_EQUAL(createByteArray("\x00", 1), createByteArray(&helper.unprocessedInput[5 + destination.size()], 1));

        helper.unprocessedInput.clear();
        serverRespondRequestFail();
        eventLoop->processEvents();
        CPPUNIT_ASSERT_EQUAL(true, helper.sessionReadyCalled);
        CPPUNIT_ASSERT_EQUAL(true, helper.sessionReadyError);
        CPPUNIT_ASSERT_EQUAL(true, connection->disconnectCalled);
    }

    void testWriteBytestream() {
        TestHelper helper;
        connection->onDataSent.connect(boost::bind(&TestHelper::handleConnectionDataWritten, &helper, _1));

        SOCKS5BytestreamClientSession::ref clientSession = std::make_shared<SOCKS5BytestreamClientSession>(connection, destinationAddressPort, destination, timerFactory.get());
        clientSession->onSessionReady.connect(boost::bind(&TestHelper::handleSessionReady, &helper, _1));

        clientSession->start();
        eventLoop->processEvents();

        helper.unprocessedInput.clear();
        serverRespondHelloOK();
        eventLoop->processEvents();

        helper.unprocessedInput.clear();
        serverRespondRequestOK();
        eventLoop->processEvents();
        CPPUNIT_ASSERT_EQUAL(true, helper.sessionReadyCalled);
        CPPUNIT_ASSERT_EQUAL(false, helper.sessionReadyError);

        std::shared_ptr<ByteArrayWriteBytestream> output = std::make_shared<ByteArrayWriteBytestream>();
        clientSession->startReceiving(output);

        ByteArray transferData = generateRandomByteArray(1024);
        connection->onDataRead(createSafeByteArrayRef(vecptr(transferData), transferData.size()));
        CPPUNIT_ASSERT_EQUAL(transferData, output->getData());
    }

    void testReadBytestream() {
        TestHelper helper;
        connection->onDataSent.connect(boost::bind(&TestHelper::handleConnectionDataWritten, &helper, _1));

        SOCKS5BytestreamClientSession::ref clientSession = std::make_shared<SOCKS5BytestreamClientSession>(connection, destinationAddressPort, destination, timerFactory.get());
        clientSession->onSessionReady.connect(boost::bind(&TestHelper::handleSessionReady, &helper, _1));

        clientSession->start();
        eventLoop->processEvents();

        helper.unprocessedInput.clear();
        serverRespondHelloOK();
        eventLoop->processEvents();

        helper.unprocessedInput.clear();
        serverRespondRequestOK();
        eventLoop->processEvents();
        CPPUNIT_ASSERT_EQUAL(true, helper.sessionReadyCalled);
        CPPUNIT_ASSERT_EQUAL(false, helper.sessionReadyError);

        helper.unprocessedInput.clear();
        ByteArray transferData = generateRandomByteArray(1024);
        std::shared_ptr<ByteArrayReadBytestream> input = std::make_shared<ByteArrayReadBytestream>(transferData);
        clientSession->startSending(input);
        eventLoop->processEvents();

        CPPUNIT_ASSERT_EQUAL(createByteArray(vecptr(transferData), transferData.size()), helper.unprocessedInput);
    }

private:
    static ByteArray generateRandomByteArray(size_t len) {
        boost::uniform_int<> dist(0, 255);
        boost::variate_generator<boost::mt19937&, boost::uniform_int<> > randomByte(randomGen, dist);
        ByteArray result(len);
        for (size_t i=0; i < len; ++i ) {
            result[i] = static_cast<unsigned char>(randomByte());
        }
        return result;
    }

    // Server responses
    void serverRespondHelloOK() {
        connection->onDataRead(createSafeByteArrayRef("\x05\00", 2));
    }

    void serverRespondHelloAuthFail() {
        connection->onDataRead(createSafeByteArrayRef("\x05\xFF", 2));
    }

    void serverRespondRequestOK() {
        std::shared_ptr<SafeByteArray> dataToSend = createSafeByteArrayRef("\x05\x00\x00\x03", 4);
        append(*dataToSend, createSafeByteArray(static_cast<char>(destination.size())));
        append(*dataToSend, createSafeByteArray(destination));
        append(*dataToSend, createSafeByteArray("\x00", 1));
        connection->onDataRead(dataToSend);
    }

    void serverRespondRequestFail() {
        std::shared_ptr<SafeByteArray> correctData = createSafeByteArrayRef("\x05\x00\x00\x03", 4);
        append(*correctData, createSafeByteArray(static_cast<char>(destination.size())));
        append(*correctData, createSafeByteArray(destination));
        append(*correctData, createSafeByteArray("\x00", 1));

        std::shared_ptr<SafeByteArray> dataToSend;
        //ByteArray failingData = Hexify::unhexify("8417947d1d305c72c11520ea7d2c6e787396705e72c312c6ccc3f66613d7cae1b91b7ab48e8b59a17d559c15fb51");
        //append(dataToSend, failingData);
        //SWIFT_LOG(debug) << "hexed: " << Hexify::hexify(failingData) << std::endl;
        do {
            ByteArray rndArray = generateRandomByteArray(correctData->size());
            dataToSend = createSafeByteArrayRef(vecptr(rndArray), rndArray.size());
        } while (*dataToSend == *correctData);
        connection->onDataRead(dataToSend);
    }

private:
    struct TestHelper {
        TestHelper() : sessionReadyCalled(false), sessionReadyError(false) {}
        ByteArray unprocessedInput;
        bool sessionReadyCalled;
        bool sessionReadyError;

        void handleConnectionDataWritten(const SafeByteArray& data) {
            append(unprocessedInput, data);
            //SWIFT_LOG(debug) << "unprocessedInput (" << unprocessedInput.size() <<  "): " << Hexify::hexify(unprocessedInput) << std::endl;
        }

        void handleSessionReady(bool error) {
            sessionReadyCalled = true;
            sessionReadyError = error;
        }
    };


private:
    struct MockeryConnection : public Connection, public EventOwner, public std::enable_shared_from_this<MockeryConnection> {
        public:
        MockeryConnection(const std::vector<HostAddressPort>& failingPorts, bool isResponsive, EventLoop* eventLoop) : eventLoop(eventLoop), failingPorts(failingPorts), isResponsive(isResponsive), disconnectCalled(false) {}

            void listen() { assert(false); }
            void connect(const HostAddressPort& address) {
                hostAddressPort = address;
                if (isResponsive) {
                    bool fail = std::find(failingPorts.begin(), failingPorts.end(), address) != failingPorts.end();
                    eventLoop->postEvent(boost::bind(boost::ref(onConnectFinished), fail));
                }
            }

            HostAddressPort getLocalAddress() const { return HostAddressPort(); }

            HostAddressPort getRemoteAddress() const { return HostAddressPort(); }

            void disconnect() {
                disconnectCalled = true;
            }

            void write(const SafeByteArray& data) {
                eventLoop->postEvent(boost::ref(onDataWritten), shared_from_this());
                onDataSent(data);
            }

            boost::signals2::signal<void (const SafeByteArray&)> onDataSent;

            EventLoop* eventLoop;
            boost::optional<HostAddressPort> hostAddressPort;
            std::vector<HostAddressPort> failingPorts;
            bool isResponsive;
            bool disconnectCalled;
    };

private:
    HostAddressPort destinationAddressPort;
    std::string destination;
    std::unique_ptr<DummyEventLoop> eventLoop;
    std::unique_ptr<DummyTimerFactory> timerFactory;
    std::shared_ptr<MockeryConnection> connection;
    const std::vector<HostAddressPort> failingPorts;
    std::shared_ptr<CryptoProvider> crypto;
};

CPPUNIT_TEST_SUITE_REGISTRATION(SOCKS5BytestreamClientSessionTest);
