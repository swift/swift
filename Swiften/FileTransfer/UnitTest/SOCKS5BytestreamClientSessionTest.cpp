/*
 * Copyright (c) 2011 Tobias Markmann
 * Licensed under the simplified BSD license.
 * See Documentation/Licenses/BSD-simplified.txt for more information.
 */

#include <Swiften/Base/ByteArray.h>
#include <QA/Checker/IO.h>

#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/extensions/TestFactoryRegistry.h>

#include <boost/bind.hpp>
#include <boost/random/mersenne_twister.hpp>
#include <boost/random/uniform_int.hpp>
#include <boost/random/variate_generator.hpp>
#include <boost/smart_ptr/make_shared.hpp>

#include <Swiften/Base/Algorithm.h>
#include <Swiften/Base/ByteArray.h>
#include <Swiften/Base/Concat.h>
#include <Swiften/Base/Log.h>
#include <Swiften/Base/StartStopper.h>
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

boost::mt19937 randomGen;

class SOCKS5BytestreamClientSessionTest : public CppUnit::TestFixture {
	CPPUNIT_TEST_SUITE(SOCKS5BytestreamClientSessionTest);
	CPPUNIT_TEST(testForSessionReady);
	CPPUNIT_TEST(testErrorHandlingHello);
	CPPUNIT_TEST(testErrorHandlingRequest);
	CPPUNIT_TEST(testWriteBytestream);
	CPPUNIT_TEST(testReadBytestream);
	CPPUNIT_TEST_SUITE_END();

	const HostAddressPort destinationAddressPort;
	const std::string destination;

public:
	SOCKS5BytestreamClientSessionTest() : destinationAddressPort(HostAddressPort(HostAddress("127.0.0.1"), 8888)),
		destination(SOCKS5BytestreamRegistry::getHostname("foo", JID("requester@example.com/test"), JID("target@example.com/test"))), eventLoop(NULL), timerFactory(NULL) { }

	void setUp() {
		randomGen.seed(time(NULL));
		eventLoop = new DummyEventLoop();
		timerFactory = new DummyTimerFactory();
		connection = boost::make_shared<MockeryConnection>(failingPorts, true, eventLoop);
		//connection->onDataSent.connect(boost::bind(&SOCKS5BytestreamServerSessionTest::handleDataWritten, this, _1));
		//stream1 = boost::make_shared<ByteArrayReadBytestream>(createByteArray("abcdefg")));
//		connection->onDataRead.connect(boost::bind(&SOCKS5BytestreamClientSessionTest::handleDataRead, this, _1));
	}

	void tearDown() {
		//connection.reset();
		delete timerFactory;
		delete eventLoop;
	}

	void testForSessionReady() {
		TestHelper helper;
		connection->onDataSent.connect(boost::bind(&TestHelper::handleConnectionDataWritten, &helper, _1));

		SOCKS5BytestreamClientSession::ref clientSession = boost::make_shared<SOCKS5BytestreamClientSession>(connection, destinationAddressPort, destination, timerFactory);
		clientSession->onSessionReady.connect(boost::bind(&TestHelper::handleSessionReady, &helper, _1));

		clientSession->start();
		eventLoop->processEvents();
		CPPUNIT_ASSERT(createByteArray("\x05\x01\x00", 3) == helper.unprocessedInput);

		helper.unprocessedInput.clear();
		serverRespondHelloOK();
		eventLoop->processEvents();
		CPPUNIT_ASSERT_EQUAL(createByteArray("\x05\x01\x00\x03", 4), createByteArray(&helper.unprocessedInput[0], 4));
		CPPUNIT_ASSERT_EQUAL(createByteArray(destination.size()), createByteArray(helper.unprocessedInput[4]));
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

		SOCKS5BytestreamClientSession::ref clientSession = boost::make_shared<SOCKS5BytestreamClientSession>(connection, destinationAddressPort, destination, timerFactory);
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

		SOCKS5BytestreamClientSession::ref clientSession = boost::make_shared<SOCKS5BytestreamClientSession>(connection, destinationAddressPort, destination, timerFactory);
		clientSession->onSessionReady.connect(boost::bind(&TestHelper::handleSessionReady, &helper, _1));

		clientSession->start();
		eventLoop->processEvents();
		CPPUNIT_ASSERT_EQUAL(createByteArray("\x05\x01\x00", 3), helper.unprocessedInput);

		helper.unprocessedInput.clear();
		serverRespondHelloOK();
		eventLoop->processEvents();
		CPPUNIT_ASSERT_EQUAL(createByteArray("\x05\x01\x00\x03", 4), createByteArray(&helper.unprocessedInput[0], 4));
		CPPUNIT_ASSERT_EQUAL(createByteArray(destination.size()), createByteArray(helper.unprocessedInput[4]));
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

		SOCKS5BytestreamClientSession::ref clientSession = boost::make_shared<SOCKS5BytestreamClientSession>(connection, destinationAddressPort, destination, timerFactory);
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

		boost::shared_ptr<ByteArrayWriteBytestream> output = boost::make_shared<ByteArrayWriteBytestream>();
		clientSession->startReceiving(output);

		ByteArray transferData = generateRandomByteArray(1024);
		connection->onDataRead(createSafeByteArrayRef(transferData.data(), transferData.size()));
		CPPUNIT_ASSERT_EQUAL(transferData, output->getData());
	}

	void testReadBytestream() {
		TestHelper helper;
		connection->onDataSent.connect(boost::bind(&TestHelper::handleConnectionDataWritten, &helper, _1));

		SOCKS5BytestreamClientSession::ref clientSession = boost::make_shared<SOCKS5BytestreamClientSession>(connection, destinationAddressPort, destination, timerFactory);
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
		boost::shared_ptr<ByteArrayReadBytestream> input = boost::make_shared<ByteArrayReadBytestream>(transferData);
		clientSession->startSending(input);
		eventLoop->processEvents();

		CPPUNIT_ASSERT_EQUAL(createByteArray(transferData.data(), transferData.size()), helper.unprocessedInput);
	}



private:
	static ByteArray generateRandomByteArray(size_t len) {
		boost::uniform_int<> dist(0, 255);
		boost::variate_generator<boost::mt19937&, boost::uniform_int<> > randomByte(randomGen, dist);
		ByteArray result(len);
		for (size_t i=0; i < len; ++i ) {
			result[i] = randomByte();
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
		boost::shared_ptr<SafeByteArray> dataToSend = createSafeByteArrayRef("\x05\x00\x00\x03", 4);
		append(*dataToSend, createSafeByteArray(destination.size()));
		append(*dataToSend, createSafeByteArray(destination));
		append(*dataToSend, createSafeByteArray("\x00", 1));
		connection->onDataRead(dataToSend);
	}

	void serverRespondRequestFail() {
		boost::shared_ptr<SafeByteArray> correctData = createSafeByteArrayRef("\x05\x00\x00\x03", 4);
		append(*correctData, createSafeByteArray(destination.size()));
		append(*correctData, createSafeByteArray(destination));
		append(*correctData, createSafeByteArray("\x00", 1));

		boost::shared_ptr<SafeByteArray> dataToSend;
		//ByteArray failingData = Hexify::unhexify("8417947d1d305c72c11520ea7d2c6e787396705e72c312c6ccc3f66613d7cae1b91b7ab48e8b59a17d559c15fb51");
		//append(dataToSend, failingData);
		//SWIFT_LOG(debug) << "hexed: " << Hexify::hexify(failingData) << std::endl;
		do {
			ByteArray rndArray = generateRandomByteArray(correctData->size());
			dataToSend = createSafeByteArrayRef(rndArray.data(), rndArray.size());
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
	struct MockeryConnection : public Connection, public EventOwner, public boost::enable_shared_from_this<MockeryConnection> {
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
			void disconnect() {
				disconnectCalled = true;
			}

			void write(const SafeByteArray& data) {
				eventLoop->postEvent(boost::ref(onDataWritten), shared_from_this());
				onDataSent(data);
			}

			boost::signal<void (const SafeByteArray&)> onDataSent;

			EventLoop* eventLoop;
			boost::optional<HostAddressPort> hostAddressPort;
			std::vector<HostAddressPort> failingPorts;
			bool isResponsive;
			bool disconnectCalled;
	};

private:
	DummyEventLoop* eventLoop;
	DummyTimerFactory* timerFactory;
	boost::shared_ptr<MockeryConnection> connection;
	const std::vector<HostAddressPort> failingPorts;
};

CPPUNIT_TEST_SUITE_REGISTRATION(SOCKS5BytestreamClientSessionTest);
