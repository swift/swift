/*
 * Copyright (c) 2010-2016 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#include <boost/bind.hpp>

#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/extensions/TestFactoryRegistry.h>

#include <Swiften/EventLoop/DummyEventLoop.h>
#include <Swiften/LinkLocal/DNSSD/DNSSDResolveHostnameQuery.h>
#include <Swiften/LinkLocal/DNSSD/DNSSDServiceID.h>
#include <Swiften/LinkLocal/DNSSD/Fake/FakeDNSSDQuerier.h>
#include <Swiften/LinkLocal/LinkLocalConnector.h>
#include <Swiften/LinkLocal/LinkLocalService.h>
#include <Swiften/Network/FakeConnection.h>

using namespace Swift;

class LinkLocalConnectorTest : public CppUnit::TestFixture {
        CPPUNIT_TEST_SUITE(LinkLocalConnectorTest);
        CPPUNIT_TEST(testConnect);
        CPPUNIT_TEST(testConnect_UnableToResolve);
        CPPUNIT_TEST(testConnect_UnableToConnect);
        CPPUNIT_TEST(testCancel_DuringResolve);
        CPPUNIT_TEST(testCancel_DuringConnect);
        CPPUNIT_TEST_SUITE_END();

    public:
        void setUp() {
            eventLoop = new DummyEventLoop();
            querier = std::make_shared<FakeDNSSDQuerier>("rabbithole.local", eventLoop);
            connection = std::make_shared<FakeConnection>(eventLoop);
            connectFinished = false;
        }

        void tearDown() {
            querier->clearAllQueriesEverRun();
            delete eventLoop;
        }

        void testConnect() {
            std::shared_ptr<LinkLocalConnector>
                    testling(createConnector("rabbithole.local", 1234));
            querier->setAddress("rabbithole.local", HostAddress::fromString("192.168.1.1").get());

            testling->connect();
            eventLoop->processEvents();

            CPPUNIT_ASSERT(connectFinished);
            CPPUNIT_ASSERT(!connectError);
            CPPUNIT_ASSERT(connection->connectedTo);
            CPPUNIT_ASSERT_EQUAL(std::string(connection->connectedTo->getAddress().toString()), std::string("192.168.1.1"));
            CPPUNIT_ASSERT_EQUAL(connection->connectedTo->getPort(), 1234);
        }

        void testConnect_UnableToResolve() {
            std::shared_ptr<LinkLocalConnector>
                    testling(createConnector("rabbithole.local", 1234));
            querier->setAddress("rabbithole.local", boost::optional<HostAddress>());

            testling->connect();
            eventLoop->processEvents();

            CPPUNIT_ASSERT(connectFinished);
            CPPUNIT_ASSERT(connectError);
            CPPUNIT_ASSERT(!connection->connectedTo);
        }

        void testConnect_UnableToConnect() {
            std::shared_ptr<LinkLocalConnector>
                    testling(createConnector("rabbithole.local", 1234));
            querier->setAddress("rabbithole.local", HostAddress::fromString("192.168.1.1").get());
            connection->setError(Connection::ReadError);

            testling->connect();
            eventLoop->processEvents();

            CPPUNIT_ASSERT(connectFinished);
            CPPUNIT_ASSERT(connectError);
            CPPUNIT_ASSERT(!connection->connectedTo);
        }

        void testCancel_DuringResolve() {
            std::shared_ptr<LinkLocalConnector>
                    testling(createConnector("rabbithole.local", 1234));
            testling->connect();
            eventLoop->processEvents();
            CPPUNIT_ASSERT(!connectFinished);

            testling->cancel();
            eventLoop->processEvents();
            querier->setAddress("rabbithole.local", HostAddress::fromString("192.168.1.1").get());
            eventLoop->processEvents();

            CPPUNIT_ASSERT(FakeConnection::Disconnected == connection->state);
        }

        void testCancel_DuringConnect() {
            std::shared_ptr<LinkLocalConnector>
                    testling(createConnector("rabbithole.local", 1234));
            querier->setAddress("rabbithole.local", HostAddress::fromString("192.168.1.1").get());
            connection->setDelayConnect();
            testling->connect();
            eventLoop->processEvents();
            CPPUNIT_ASSERT(FakeConnection::Connecting == connection->state);

            testling->cancel();
            eventLoop->processEvents();

            CPPUNIT_ASSERT(FakeConnection::Disconnected == connection->state);
        }

    private:
        std::shared_ptr<LinkLocalConnector> createConnector(const std::string& hostname, int port) {
            LinkLocalService service(
                    DNSSDServiceID("myname", "local."),
                    DNSSDResolveServiceQuery::Result(
                        "myname._presence._tcp.local", hostname, port,
                        LinkLocalServiceInfo().toTXTRecord()));
            std::shared_ptr<LinkLocalConnector> result(
                    new LinkLocalConnector(service, querier, connection));
            result->onConnectFinished.connect(
                    boost::bind(&LinkLocalConnectorTest::handleConnected, this, _1));
            return result;
        }

        void handleConnected(bool e) {
            connectFinished = true;
            connectError = e;
        }

    private:
        DummyEventLoop* eventLoop;
        std::shared_ptr<FakeDNSSDQuerier> querier;
        std::shared_ptr<FakeConnection> connection;
        bool connectFinished;
        bool connectError;
};

CPPUNIT_TEST_SUITE_REGISTRATION(LinkLocalConnectorTest);
