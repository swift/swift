/*
 * Copyright (c) 2010-2016 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#include <boost/bind.hpp>
#include <boost/optional.hpp>

#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/extensions/TestFactoryRegistry.h>

#include <Swiften/Component/ComponentConnector.h>
#include <Swiften/EventLoop/DummyEventLoop.h>
#include <Swiften/Network/Connection.h>
#include <Swiften/Network/ConnectionFactory.h>
#include <Swiften/Network/DummyTimerFactory.h>
#include <Swiften/Network/HostAddressPort.h>
#include <Swiften/Network/StaticDomainNameResolver.h>

using namespace Swift;

class ComponentConnectorTest : public CppUnit::TestFixture {
        CPPUNIT_TEST_SUITE(ComponentConnectorTest);
        CPPUNIT_TEST(testConnect);
        CPPUNIT_TEST(testConnect_FirstAddressHostFails);
        CPPUNIT_TEST(testConnect_NoHosts);
        CPPUNIT_TEST(testConnect_TimeoutDuringResolve);
        CPPUNIT_TEST(testConnect_TimeoutDuringConnect);
        CPPUNIT_TEST(testConnect_NoTimeout);
        CPPUNIT_TEST(testStop_Timeout);
        CPPUNIT_TEST_SUITE_END();

    public:
        void setUp() {
            host1 = HostAddress::fromString("1.1.1.1").get();
            host2 = HostAddress::fromString("2.2.2.2").get();
            eventLoop = new DummyEventLoop();
            resolver = new StaticDomainNameResolver(eventLoop);
            connectionFactory = new MockConnectionFactory(eventLoop);
            timerFactory = new DummyTimerFactory();
        }

        void tearDown() {
            delete timerFactory;
            delete connectionFactory;
            delete resolver;
            delete eventLoop;
        }

        void testConnect() {
            ComponentConnector::ref testling(createConnector("foo.com", 1234));
            resolver->addAddress("foo.com", host1);

            testling->start();
            eventLoop->processEvents();

            CPPUNIT_ASSERT_EQUAL(1, static_cast<int>(connections.size()));
            CPPUNIT_ASSERT(connections[0]);
            CPPUNIT_ASSERT(HostAddressPort(host1, 1234) == *(connections[0]->hostAddressPort));
        }

        void testConnect_FirstAddressHostFails() {
            ComponentConnector::ref testling(createConnector("foo.com", 1234));
            resolver->addAddress("foo.com", host1);
            resolver->addAddress("foo.com", host2);
            connectionFactory->failingPorts.push_back(HostAddressPort(host1, 1234));

            testling->start();
            eventLoop->processEvents();

            CPPUNIT_ASSERT_EQUAL(1, static_cast<int>(connections.size()));
            CPPUNIT_ASSERT(connections[0]);
            CPPUNIT_ASSERT(HostAddressPort(host2, 1234) == *(connections[0]->hostAddressPort));
        }

        void testConnect_NoHosts() {
            ComponentConnector::ref testling(createConnector("foo.com", 1234));

            testling->start();
            eventLoop->processEvents();

            CPPUNIT_ASSERT_EQUAL(1, static_cast<int>(connections.size()));
            CPPUNIT_ASSERT(!connections[0]);
        }


        void testConnect_TimeoutDuringResolve() {
            ComponentConnector::ref testling(createConnector("foo.com", 1234));

            testling->setTimeoutMilliseconds(10);
            resolver->setIsResponsive(false);

            testling->start();
            eventLoop->processEvents();
            timerFactory->setTime(10);
            eventLoop->processEvents();

            CPPUNIT_ASSERT_EQUAL(1, static_cast<int>(connections.size()));
            CPPUNIT_ASSERT(!connections[0]);
        }

        void testConnect_TimeoutDuringConnect() {
            ComponentConnector::ref testling(createConnector("foo.com", 1234));
            testling->setTimeoutMilliseconds(10);
            resolver->addAddress("foo.com", host1);
            connectionFactory->isResponsive = false;

            testling->start();
            eventLoop->processEvents();
            timerFactory->setTime(10);
            eventLoop->processEvents();

            CPPUNIT_ASSERT_EQUAL(1, static_cast<int>(connections.size()));
            CPPUNIT_ASSERT(!connections[0]);
        }

        void testConnect_NoTimeout() {
            ComponentConnector::ref testling(createConnector("foo.com", 1234));
            testling->setTimeoutMilliseconds(10);
            resolver->addAddress("foo.com", host1);

            testling->start();
            eventLoop->processEvents();
            timerFactory->setTime(10);
            eventLoop->processEvents();

            CPPUNIT_ASSERT_EQUAL(1, static_cast<int>(connections.size()));
            CPPUNIT_ASSERT(connections[0]);
        }

        void testStop_Timeout() {
            ComponentConnector::ref testling(createConnector("foo.com", 1234));
            testling->setTimeoutMilliseconds(10);
            resolver->addAddress("foo.com", host1);

            testling->start();
            testling->stop();

            eventLoop->processEvents();
            timerFactory->setTime(10);
            eventLoop->processEvents();

            CPPUNIT_ASSERT_EQUAL(1, static_cast<int>(connections.size()));
            CPPUNIT_ASSERT(!connections[0]);
        }

    private:
        ComponentConnector::ref createConnector(const std::string& hostname, int port) {
            ComponentConnector::ref connector = ComponentConnector::create(hostname, port, resolver, connectionFactory, timerFactory);
            connector->onConnectFinished.connect(boost::bind(&ComponentConnectorTest::handleConnectorFinished, this, _1));
            return connector;
        }

        void handleConnectorFinished(std::shared_ptr<Connection> connection) {
            std::shared_ptr<MockConnection> c(std::dynamic_pointer_cast<MockConnection>(connection));
            if (connection) {
                assert(c);
            }
            connections.push_back(c);
        }

        struct MockConnection : public Connection {
            public:
                MockConnection(const std::vector<HostAddressPort>& failingPorts, bool isResponsive, EventLoop* eventLoop) : eventLoop(eventLoop), failingPorts(failingPorts), isResponsive(isResponsive) {}

                void listen() { assert(false); }
                void connect(const HostAddressPort& address) {
                    hostAddressPort = address;
                    if (isResponsive) {
                        bool fail = std::find(failingPorts.begin(), failingPorts.end(), address) != failingPorts.end();
                        eventLoop->postEvent(boost::bind(boost::ref(onConnectFinished), fail));
                    }
                }

                void disconnect() { assert(false); }
                void write(const SafeByteArray&) { assert(false); }
                HostAddressPort getLocalAddress() const { return HostAddressPort(); }
                HostAddressPort getRemoteAddress() const { return HostAddressPort(); }

                EventLoop* eventLoop;
                boost::optional<HostAddressPort> hostAddressPort;
                std::vector<HostAddressPort> failingPorts;
                bool isResponsive;
        };

        struct MockConnectionFactory : public ConnectionFactory {
            MockConnectionFactory(EventLoop* eventLoop) : eventLoop(eventLoop), isResponsive(true) {
            }

            std::shared_ptr<Connection> createConnection() {
                return std::make_shared<MockConnection>(failingPorts, isResponsive, eventLoop);
            }

            EventLoop* eventLoop;
            bool isResponsive;
            std::vector<HostAddressPort> failingPorts;
        };

    private:
        HostAddress host1;
        HostAddress host2;
        DummyEventLoop* eventLoop;
        StaticDomainNameResolver* resolver;
        MockConnectionFactory* connectionFactory;
        DummyTimerFactory* timerFactory;
        std::vector< std::shared_ptr<MockConnection> > connections;
};

CPPUNIT_TEST_SUITE_REGISTRATION(ComponentConnectorTest);
