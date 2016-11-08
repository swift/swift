/*
 * Copyright (c) 2010-2016 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#include <memory>
#include <string>

#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/extensions/TestFactoryRegistry.h>

#include <Swiften/Base/sleep.h>
#include <Swiften/EventLoop/DummyEventLoop.h>
#include <Swiften/Network/BoostConnectionServer.h>
#include <Swiften/Network/BoostIOServiceThread.h>

using namespace Swift;

class BoostConnectionServerTest : public CppUnit::TestFixture {
        CPPUNIT_TEST_SUITE(BoostConnectionServerTest);
        CPPUNIT_TEST(testConstructor_TwoServersOnSamePort);
        CPPUNIT_TEST(testStart_Conflict);
        CPPUNIT_TEST(testStop);
        CPPUNIT_TEST(testIPv4Server);
        CPPUNIT_TEST(testIPv6Server);
        CPPUNIT_TEST(testIPv4IPv6DualStackServer);
        CPPUNIT_TEST(testIPv6DualStackServerPeerAddress);
        CPPUNIT_TEST_SUITE_END();

    public:
        void setUp() {
            eventLoop_ = new DummyEventLoop();
            boostIOServiceThread_ = new BoostIOServiceThread();
            stopped_ = false;
            stoppedError_.reset();
            receivedNewConnection_ = false;
            connectFinished_ = false;
            remoteAddress_ = boost::optional<HostAddressPort>();
        }

        void tearDown() {
            delete boostIOServiceThread_;
            while (eventLoop_->hasEvents()) {
                eventLoop_->processEvents();
            }
            delete eventLoop_;
        }

        void testConstructor_TwoServersOnSamePort() {
            BoostConnectionServer::ref testling(BoostConnectionServer::create(9999, boostIOServiceThread_->getIOService(), eventLoop_));
            BoostConnectionServer::ref testling2(BoostConnectionServer::create(9999, boostIOServiceThread_->getIOService(), eventLoop_));
        }

        void testStart_Conflict() {
            BoostConnectionServer::ref testling(BoostConnectionServer::create(9999, boostIOServiceThread_->getIOService(), eventLoop_));
            testling->start();

            BoostConnectionServer::ref testling2(BoostConnectionServer::create(9999, boostIOServiceThread_->getIOService(), eventLoop_));
            testling2->onStopped.connect(
                    boost::bind(&BoostConnectionServerTest::handleStopped_, this, _1));

            testling->stop();
        }

        void testStop() {
            BoostConnectionServer::ref testling(BoostConnectionServer::create(9999, boostIOServiceThread_->getIOService(), eventLoop_));
            testling->start();

            testling->stop();

            BoostConnectionServer::ref testling2(BoostConnectionServer::create(9999, boostIOServiceThread_->getIOService(), eventLoop_));
            testling2->start();

            testling2->stop();
        }

        void testIPv4Server() {
            BoostConnectionServer::ref testling = BoostConnectionServer::create(HostAddress::fromString("127.0.0.1").get(), 9999, boostIOServiceThread_->getIOService(), eventLoop_);
            testling->onNewConnection.connect(boost::bind(&BoostConnectionServerTest::handleNewConnection, this, _1));
            testling->start();

            BoostConnection::ref clientTestling = BoostConnection::create(boostIOServiceThread_->getIOService(), eventLoop_);
            clientTestling->onConnectFinished.connect(boost::bind(&BoostConnectionServerTest::handleConnectFinished, this, _1));
            clientTestling->connect(HostAddressPort(HostAddress::fromString("127.0.0.1").get(), 9999));

            while (!connectFinished_) {
                Swift::sleep(10);
                eventLoop_->processEvents();
            }

            CPPUNIT_ASSERT_EQUAL(true, receivedNewConnection_);

            testling->stop();
        }

        void testIPv6Server() {
            BoostConnectionServer::ref testling = BoostConnectionServer::create(HostAddress::fromString("::1").get(), 9999, boostIOServiceThread_->getIOService(), eventLoop_);
            testling->onNewConnection.connect(boost::bind(&BoostConnectionServerTest::handleNewConnection, this, _1));
            testling->start();

            BoostConnection::ref clientTestling = BoostConnection::create(boostIOServiceThread_->getIOService(), eventLoop_);
            clientTestling->onConnectFinished.connect(boost::bind(&BoostConnectionServerTest::handleConnectFinished, this, _1));
            clientTestling->connect(HostAddressPort(HostAddress::fromString("::1").get(), 9999));

            while (!connectFinished_) {
                Swift::sleep(10);
                eventLoop_->processEvents();
            }

            CPPUNIT_ASSERT_EQUAL(true, receivedNewConnection_);

            testling->stop();
        }

        void testIPv4IPv6DualStackServer() {
            BoostConnectionServer::ref testling = BoostConnectionServer::create(HostAddress::fromString("::").get(), 9999, boostIOServiceThread_->getIOService(), eventLoop_);
            testling->onNewConnection.connect(boost::bind(&BoostConnectionServerTest::handleNewConnection, this, _1));
            testling->start();

            // Test IPv4.
            BoostConnection::ref clientTestling = BoostConnection::create(boostIOServiceThread_->getIOService(), eventLoop_);
            clientTestling->onConnectFinished.connect(boost::bind(&BoostConnectionServerTest::handleConnectFinished, this, _1));
            clientTestling->connect(HostAddressPort(HostAddress::fromString("127.0.0.1").get(), 9999));

            while (!connectFinished_) {
                Swift::sleep(10);
                eventLoop_->processEvents();
            }

            CPPUNIT_ASSERT_EQUAL(true, receivedNewConnection_);

            receivedNewConnection_ = false;
            connectFinished_ = false;

            // Test IPv6.
            clientTestling = BoostConnection::create(boostIOServiceThread_->getIOService(), eventLoop_);
            clientTestling->onConnectFinished.connect(boost::bind(&BoostConnectionServerTest::handleConnectFinished, this, _1));
            clientTestling->connect(HostAddressPort(HostAddress::fromString("::1").get(), 9999));

            while (!connectFinished_) {
                Swift::sleep(10);
                eventLoop_->processEvents();
            }

            CPPUNIT_ASSERT_EQUAL(true, receivedNewConnection_);

            testling->stop();
        }

        void testIPv6DualStackServerPeerAddress() {
            BoostConnectionServer::ref testling = BoostConnectionServer::create(HostAddress::fromString("::").get(), 9999, boostIOServiceThread_->getIOService(), eventLoop_);
            testling->onNewConnection.connect(boost::bind(&BoostConnectionServerTest::handleNewConnection, this, _1));
            testling->start();

            // Test IPv4.
            BoostConnection::ref clientTestling = BoostConnection::create(boostIOServiceThread_->getIOService(), eventLoop_);
            clientTestling->onConnectFinished.connect(boost::bind(&BoostConnectionServerTest::handleConnectFinished, this, _1));
            clientTestling->connect(HostAddressPort(HostAddress::fromString("127.0.0.1").get(), 9999));

            while (!connectFinished_) {
                Swift::sleep(10);
                eventLoop_->processEvents();
            }

            CPPUNIT_ASSERT_EQUAL(true, receivedNewConnection_);
            // The IPv4 localhost mapped to a IPv6 address is expected here.
            CPPUNIT_ASSERT(HostAddress::fromString("::ffff:127.0.0.1").get() == remoteAddress_.get().getAddress());

            receivedNewConnection_ = false;
            connectFinished_ = false;
            remoteAddress_ = boost::optional<HostAddressPort>();

            // Test IPv6.
            clientTestling = BoostConnection::create(boostIOServiceThread_->getIOService(), eventLoop_);
            clientTestling->onConnectFinished.connect(boost::bind(&BoostConnectionServerTest::handleConnectFinished, this, _1));
            clientTestling->connect(HostAddressPort(HostAddress::fromString("::1").get(), 9999));

            while (!connectFinished_) {
                Swift::sleep(10);
                eventLoop_->processEvents();
            }

            CPPUNIT_ASSERT_EQUAL(true, receivedNewConnection_);
            // The IPv6 local host is expected here.
            CPPUNIT_ASSERT(HostAddress::fromString("::1").get() == remoteAddress_.get().getAddress());

            testling->stop();
        }

        void handleStopped_(boost::optional<BoostConnectionServer::Error> e) {
            stopped_ = true;
            stoppedError_ = e;
        }

        void handleNewConnection(std::shared_ptr<Connection> connection) {
            receivedNewConnection_ = true;
            remoteAddress_ = connection->getRemoteAddress();
        }

        void handleConnectFinished(bool /*error*/) {
            connectFinished_ = true;
        }

    private:
        BoostIOServiceThread* boostIOServiceThread_;
        DummyEventLoop* eventLoop_;
        bool stopped_;
        bool receivedNewConnection_;
        bool connectFinished_;
        boost::optional<BoostConnectionServer::Error> stoppedError_;
        boost::optional<HostAddressPort> remoteAddress_;
};

CPPUNIT_TEST_SUITE_REGISTRATION(BoostConnectionServerTest);
