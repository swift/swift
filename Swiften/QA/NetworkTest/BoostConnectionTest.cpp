/*
 * Copyright (c) 2010 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/extensions/TestFactoryRegistry.h>
#include <boost/shared_ptr.hpp>
#include <boost/smart_ptr/make_shared.hpp>

#include <string>
#include <Swiften/Base/Algorithm.h>
#include <Swiften/Base/sleep.h>
#include <Swiften/Network/BoostConnection.h>
#include <Swiften/Network/HostAddress.h>
#include <Swiften/Network/HostAddressPort.h>
#include <Swiften/Network/BoostIOServiceThread.h>
#include <Swiften/EventLoop/DummyEventLoop.h>

const unsigned char* address = reinterpret_cast<const unsigned char*>("\x41\x63\xde\x89");

using namespace Swift;

class BoostConnectionTest : public CppUnit::TestFixture {
		CPPUNIT_TEST_SUITE(BoostConnectionTest);
		CPPUNIT_TEST(testDestructor);
		CPPUNIT_TEST(testDestructor_PendingEvents);
		CPPUNIT_TEST(testWrite);
		CPPUNIT_TEST(testWriteMultipleSimultaniouslyQueuesWrites);
#ifdef TEST_IPV6
		CPPUNIT_TEST(testWrite_IPv6);
#endif
		CPPUNIT_TEST_SUITE_END();

	public:
		void setUp() {
			boostIOServiceThread_ = new BoostIOServiceThread();
			boostIOService = boost::make_shared<boost::asio::io_service>();
			eventLoop_ = new DummyEventLoop();
			disconnected = false;
			connectFinished = false;
		}

		void tearDown() {
			delete eventLoop_;
			delete boostIOServiceThread_;
		}

		void testDestructor() {
			{
				BoostConnection::ref testling(BoostConnection::create(boostIOServiceThread_->getIOService(), eventLoop_));
				testling->connect(HostAddressPort(HostAddress(address, 4), 5222));
			}
		}

		void testDestructor_PendingEvents() {
			{
				BoostConnection::ref testling(BoostConnection::create(boostIOServiceThread_->getIOService(), eventLoop_));
				testling->connect(HostAddressPort(HostAddress(address, 4), 5222));
				while (!eventLoop_->hasEvents()) {
					Swift::sleep(10);
				}
			}
			eventLoop_->processEvents();
		}

		void testWrite() {
			BoostConnection::ref testling(BoostConnection::create(boostIOServiceThread_->getIOService(), eventLoop_));
			testling->onConnectFinished.connect(boost::bind(&BoostConnectionTest::doWrite, this, testling.get()));
			testling->onDataRead.connect(boost::bind(&BoostConnectionTest::handleDataRead, this, _1));
			testling->onDisconnected.connect(boost::bind(&BoostConnectionTest::handleDisconnected, this));
			testling->connect(HostAddressPort(HostAddress("65.99.222.137"), 5222));
			while (receivedData.empty()) {
				Swift::sleep(10);
				eventLoop_->processEvents();
			}
			testling->disconnect();
		}

		void testWrite_IPv6() {
			BoostConnection::ref testling(BoostConnection::create(boostIOServiceThread_->getIOService(), eventLoop_));
			testling->onConnectFinished.connect(boost::bind(&BoostConnectionTest::doWrite, this, testling.get()));
			testling->onDataRead.connect(boost::bind(&BoostConnectionTest::handleDataRead, this, _1));
			testling->onDisconnected.connect(boost::bind(&BoostConnectionTest::handleDisconnected, this));
			testling->connect(HostAddressPort(HostAddress("2001:470:1f0e:852::2"), 80));
			while (receivedData.empty()) {
				Swift::sleep(10);
				eventLoop_->processEvents();
			}
			testling->disconnect();
		}


		void testWriteMultipleSimultaniouslyQueuesWrites() {
			BoostConnection::ref testling(BoostConnection::create(boostIOService, eventLoop_));
			testling->onConnectFinished.connect(boost::bind(&BoostConnectionTest::handleConnectFinished, this));
			testling->onDataRead.connect(boost::bind(&BoostConnectionTest::handleDataRead, this, _1));
			testling->onDisconnected.connect(boost::bind(&BoostConnectionTest::handleDisconnected, this));
			testling->connect(HostAddressPort(HostAddress("65.99.222.137"), 5222));
			while (!connectFinished) {
				boostIOService->run_one();
				eventLoop_->processEvents();
			}

			testling->write(createSafeByteArray("<stream:strea"));
			testling->write(createSafeByteArray("m"));
			testling->write(createSafeByteArray(">"));

			 // Check that we only did one write event, the others are queued
			/*int runHandlers = */boostIOService->poll();
			// Disabling this test, because poll runns all handlers that are added during poll() as well, so
			// this test doesn't really work any more. We'll have to trust that things are queued.
			//CPPUNIT_ASSERT_EQUAL(1, runHandlers);
			// Process the other events
			while (receivedData.empty()) {
				boostIOService->run_one();
				eventLoop_->processEvents();
			}

			// Disconnect & clean up
			testling->disconnect();
			while (!disconnected) {
				boostIOService->run_one();
				eventLoop_->processEvents();
			}
		}

		void doWrite(BoostConnection* connection) {
			connection->write(createSafeByteArray("<stream:stream>"));
			connection->write(createSafeByteArray("\r\n\r\n")); // Temporarily, while we don't have an xmpp server running on ipv6
		}

		void handleDataRead(boost::shared_ptr<SafeByteArray> data) {
			append(receivedData, *data);
		}

		void handleDisconnected() {
			disconnected = true;
		}

		void handleConnectFinished() {
			connectFinished = true;
		}

	private:
		BoostIOServiceThread* boostIOServiceThread_;
		boost::shared_ptr<boost::asio::io_service> boostIOService;
		DummyEventLoop* eventLoop_;
		ByteArray receivedData;
		bool disconnected;
		bool connectFinished;
};

CPPUNIT_TEST_SUITE_REGISTRATION(BoostConnectionTest);
