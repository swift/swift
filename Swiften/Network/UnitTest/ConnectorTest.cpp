/*
 * Copyright (c) 2010-2016 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#include <boost/bind.hpp>
#include <boost/optional.hpp>

#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/extensions/TestFactoryRegistry.h>

#include <Swiften/EventLoop/DummyEventLoop.h>
#include <Swiften/Network/Connection.h>
#include <Swiften/Network/ConnectionFactory.h>
#include <Swiften/Network/Connector.h>
#include <Swiften/Network/DomainNameAddressQuery.h>
#include <Swiften/Network/DummyTimerFactory.h>
#include <Swiften/Network/HostAddressPort.h>
#include <Swiften/Network/StaticDomainNameResolver.h>

using namespace Swift;

class ConnectorTest : public CppUnit::TestFixture {
        CPPUNIT_TEST_SUITE(ConnectorTest);
        CPPUNIT_TEST(testConnect);
        CPPUNIT_TEST(testConnect_NoServiceLookups);
        CPPUNIT_TEST(testConnect_NoServiceLookups_DefaultPort);
        CPPUNIT_TEST(testConnect_FirstAddressHostFails);
        CPPUNIT_TEST(testConnect_NoSRVHost);
        CPPUNIT_TEST(testConnect_NoHosts);
        CPPUNIT_TEST(testConnect_FirstSRVHostFails);
        CPPUNIT_TEST(testConnect_AllSRVHostsFailWithoutFallbackHost);
        CPPUNIT_TEST(testConnect_AllSRVHostsFailWithFallbackHost);
        CPPUNIT_TEST(testConnect_SRVAndFallbackHostsFail);
        //CPPUNIT_TEST(testConnect_TimeoutDuringResolve);
        CPPUNIT_TEST(testConnect_TimeoutDuringConnectToOnlyCandidate);
        CPPUNIT_TEST(testConnect_TimeoutDuringConnectToCandidateFallsBack);
        CPPUNIT_TEST(testConnect_NoTimeout);
        CPPUNIT_TEST(testStop_DuringSRVQuery);
        CPPUNIT_TEST(testStop_Timeout);
        CPPUNIT_TEST_SUITE_END();

    public:
        void setUp() {
            host1 = HostAddressPort(HostAddress::fromString("1.1.1.1").get(), 1234);
            host2 = HostAddressPort(HostAddress::fromString("2.2.2.2").get(), 2345);
            host3 = HostAddressPort(HostAddress::fromString("3.3.3.3").get(), 5222);
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
            Connector::ref testling(createConnector());
            resolver->addXMPPClientService("foo.com", host1);
            resolver->addXMPPClientService("foo.com", host2);
            resolver->addAddress("foo.com", host3.getAddress());

            testling->start();
            eventLoop->processEvents();

            CPPUNIT_ASSERT_EQUAL(1, static_cast<int>(connections.size()));
            CPPUNIT_ASSERT(connections[0]);
            CPPUNIT_ASSERT(host1 == *(connections[0]->hostAddressPort));
            CPPUNIT_ASSERT(!std::dynamic_pointer_cast<DomainNameResolveError>(error));
        }

        void testConnect_NoServiceLookups() {
            Connector::ref testling(createConnector(4321, boost::optional<std::string>()));
            resolver->addXMPPClientService("foo.com", host1);
            resolver->addXMPPClientService("foo.com", host2);
            resolver->addAddress("foo.com", host3.getAddress());

            testling->start();
            eventLoop->processEvents();

            CPPUNIT_ASSERT_EQUAL(1, static_cast<int>(connections.size()));
            CPPUNIT_ASSERT(connections[0]);
            CPPUNIT_ASSERT(host3.getAddress() == (*(connections[0]->hostAddressPort)).getAddress());
            CPPUNIT_ASSERT(4321 == (*(connections[0]->hostAddressPort)).getPort());
            CPPUNIT_ASSERT(!std::dynamic_pointer_cast<DomainNameResolveError>(error));
        }

        void testConnect_NoServiceLookups_DefaultPort() {
            Connector::ref testling(createConnector(-1, boost::optional<std::string>()));
            resolver->addXMPPClientService("foo.com", host1);
            resolver->addXMPPClientService("foo.com", host2);
            resolver->addAddress("foo.com", host3.getAddress());

            testling->start();
            eventLoop->processEvents();

            CPPUNIT_ASSERT_EQUAL(1, static_cast<int>(connections.size()));
            CPPUNIT_ASSERT(connections[0]);
            CPPUNIT_ASSERT(host3.getAddress() == (*(connections[0]->hostAddressPort)).getAddress());
            CPPUNIT_ASSERT_EQUAL(5222, (*(connections[0]->hostAddressPort)).getPort());
            CPPUNIT_ASSERT(!std::dynamic_pointer_cast<DomainNameResolveError>(error));
        }

        void testConnect_NoSRVHost() {
            Connector::ref testling(createConnector());
            resolver->addAddress("foo.com", host3.getAddress());

            testling->start();
            eventLoop->processEvents();

            CPPUNIT_ASSERT_EQUAL(1, static_cast<int>(connections.size()));
            CPPUNIT_ASSERT(connections[0]);
            CPPUNIT_ASSERT(host3 == *(connections[0]->hostAddressPort));
            CPPUNIT_ASSERT(!std::dynamic_pointer_cast<DomainNameResolveError>(error));
        }

        void testConnect_FirstAddressHostFails() {
            Connector::ref testling(createConnector());

            auto address1 = HostAddress::fromString("1.1.1.1").get();
            auto address2 = HostAddress::fromString("2.2.2.2").get();
            resolver->addXMPPClientService("foo.com", "host-foo.com", 1234);
            resolver->addAddress("host-foo.com", address1);
            resolver->addAddress("host-foo.com", address2);
            connectionFactory->failingPorts.push_back(HostAddressPort(address1, 1234));

            testling->start();
            eventLoop->processEvents();

            CPPUNIT_ASSERT_EQUAL(1, static_cast<int>(connections.size()));
            CPPUNIT_ASSERT(connections[0]);
            CPPUNIT_ASSERT(HostAddressPort(address2, 1234) == *(connections[0]->hostAddressPort));
            CPPUNIT_ASSERT(!std::dynamic_pointer_cast<DomainNameResolveError>(error));
        }

        void testConnect_NoHosts() {
            Connector::ref testling(createConnector());

            testling->start();
            eventLoop->processEvents();

            CPPUNIT_ASSERT_EQUAL(1, static_cast<int>(connections.size()));
            CPPUNIT_ASSERT(!connections[0]);
            CPPUNIT_ASSERT(std::dynamic_pointer_cast<DomainNameResolveError>(error));
        }

        void testConnect_FirstSRVHostFails() {
            Connector::ref testling(createConnector());
            resolver->addXMPPClientService("foo.com", host1);
            resolver->addXMPPClientService("foo.com", host2);
            connectionFactory->failingPorts.push_back(host1);

            testling->start();
            eventLoop->processEvents();

            CPPUNIT_ASSERT_EQUAL(1, static_cast<int>(connections.size()));
            CPPUNIT_ASSERT(host2 == *(connections[0]->hostAddressPort));
            CPPUNIT_ASSERT(!std::dynamic_pointer_cast<DomainNameResolveError>(error));
        }

        void testConnect_AllSRVHostsFailWithoutFallbackHost() {
            Connector::ref testling(createConnector());
            resolver->addXMPPClientService("foo.com", host1);
            resolver->addXMPPClientService("foo.com", host2);
            connectionFactory->failingPorts.push_back(host1);
            connectionFactory->failingPorts.push_back(host2);

            testling->start();
            eventLoop->processEvents();

            CPPUNIT_ASSERT_EQUAL(1, static_cast<int>(connections.size()));
            CPPUNIT_ASSERT(!connections[0]);
            CPPUNIT_ASSERT(!std::dynamic_pointer_cast<DomainNameResolveError>(error));
        }

        void testConnect_AllSRVHostsFailWithFallbackHost() {
            Connector::ref testling(createConnector());
            resolver->addXMPPClientService("foo.com", host1);
            resolver->addXMPPClientService("foo.com", host2);
            resolver->addAddress("foo.com", host3.getAddress());
            connectionFactory->failingPorts.push_back(host1);
            connectionFactory->failingPorts.push_back(host2);

            testling->start();
            eventLoop->processEvents();

            CPPUNIT_ASSERT_EQUAL(1, static_cast<int>(connections.size()));
            CPPUNIT_ASSERT(connections[0]);
            CPPUNIT_ASSERT(host3 == *(connections[0]->hostAddressPort));
            CPPUNIT_ASSERT(!std::dynamic_pointer_cast<DomainNameResolveError>(error));
        }

        void testConnect_SRVAndFallbackHostsFail() {
            Connector::ref testling(createConnector());
            resolver->addXMPPClientService("foo.com", host1);
            resolver->addAddress("foo.com", host3.getAddress());
            connectionFactory->failingPorts.push_back(host1);
            connectionFactory->failingPorts.push_back(host3);

            testling->start();
            eventLoop->processEvents();

            CPPUNIT_ASSERT_EQUAL(1, static_cast<int>(connections.size()));
            CPPUNIT_ASSERT(!connections[0]);
            CPPUNIT_ASSERT(!std::dynamic_pointer_cast<DomainNameResolveError>(error));
        }

        /*void testConnect_TimeoutDuringResolve() {
            Connector::ref testling(createConnector());
            testling->setTimeoutMilliseconds(10);
            resolver->setIsResponsive(false);

            testling->start();
            eventLoop->processEvents();
            timerFactory->setTime(10);
            eventLoop->processEvents();

            CPPUNIT_ASSERT_EQUAL(1, static_cast<int>(connections.size()));
            CPPUNIT_ASSERT(std::dynamic_pointer_cast<DomainNameResolveError>(error));
            CPPUNIT_ASSERT(!connections[0]);
        }*/

        void testConnect_TimeoutDuringConnectToOnlyCandidate() {
            Connector::ref testling(createConnector());
            testling->setTimeoutMilliseconds(10);
            resolver->addXMPPClientService("foo.com", host1);
            connectionFactory->isResponsive = false;

            testling->start();
            eventLoop->processEvents();
            timerFactory->setTime(10);
            eventLoop->processEvents();

            CPPUNIT_ASSERT_EQUAL(1, static_cast<int>(connections.size()));
            CPPUNIT_ASSERT(!connections[0]);
            CPPUNIT_ASSERT(!std::dynamic_pointer_cast<DomainNameResolveError>(error));
        }

        void testConnect_TimeoutDuringConnectToCandidateFallsBack() {
            Connector::ref testling(createConnector());
            testling->setTimeoutMilliseconds(10);

            auto address2 = HostAddress::fromString("2.2.2.2").get();

            resolver->addXMPPClientService("foo.com", "host-foo.com", 1234);
            resolver->addAddress("host-foo.com", HostAddress::fromString("1.1.1.1").get());
            resolver->addAddress("host-foo.com", address2);

            connectionFactory->isResponsive = false;
            testling->start();
            eventLoop->processEvents();
            connectionFactory->isResponsive = true;
            timerFactory->setTime(10);
            eventLoop->processEvents();

            CPPUNIT_ASSERT_EQUAL(1, static_cast<int>(connections.size()));
            CPPUNIT_ASSERT(connections[0]);
            CPPUNIT_ASSERT(HostAddressPort(address2, 1234) == *(connections[0]->hostAddressPort));
            CPPUNIT_ASSERT(!std::dynamic_pointer_cast<DomainNameResolveError>(error));
        }


        void testConnect_NoTimeout() {
            Connector::ref testling(createConnector());
            testling->setTimeoutMilliseconds(10);
            resolver->addXMPPClientService("foo.com", host1);

            testling->start();
            eventLoop->processEvents();
            timerFactory->setTime(10);
            eventLoop->processEvents();

            CPPUNIT_ASSERT_EQUAL(1, static_cast<int>(connections.size()));
            CPPUNIT_ASSERT(connections[0]);
            CPPUNIT_ASSERT(!std::dynamic_pointer_cast<DomainNameResolveError>(error));
        }

        void testStop_DuringSRVQuery() {
            Connector::ref testling(createConnector());
            resolver->addXMPPClientService("foo.com", host1);

            testling->start();
            testling->stop();

            eventLoop->processEvents();

            CPPUNIT_ASSERT_EQUAL(1, static_cast<int>(connections.size()));
            CPPUNIT_ASSERT(!connections[0]);
            CPPUNIT_ASSERT(std::dynamic_pointer_cast<DomainNameResolveError>(error));
        }

        void testStop_Timeout() {
            Connector::ref testling(createConnector());
            testling->setTimeoutMilliseconds(10);
            resolver->addXMPPClientService("foo.com", host1);

            testling->start();
            testling->stop();

            eventLoop->processEvents();
            timerFactory->setTime(10);
            eventLoop->processEvents();

            CPPUNIT_ASSERT_EQUAL(1, static_cast<int>(connections.size()));
            CPPUNIT_ASSERT(!connections[0]);
        }


    private:
        Connector::ref createConnector(int port = -1, boost::optional<std::string> serviceLookupPrefix = boost::optional<std::string>("_xmpp-client._tcp.")) {
            Connector::ref connector = Connector::create("foo.com", port, serviceLookupPrefix, resolver, connectionFactory, timerFactory);
            connector->onConnectFinished.connect(boost::bind(&ConnectorTest::handleConnectorFinished, this, _1, _2));
            return connector;
        }

        void handleConnectorFinished(std::shared_ptr<Connection> connection, std::shared_ptr<Error> resultError) {
            std::shared_ptr<MockConnection> c(std::dynamic_pointer_cast<MockConnection>(connection));
            if (connection) {
                assert(c);
            }
            connections.push_back(c);
            error = resultError;
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

                HostAddressPort getLocalAddress() const { return HostAddressPort(); }
                HostAddressPort getRemoteAddress() const { return HostAddressPort(); }
                void disconnect() { assert(false); }
                void write(const SafeByteArray&) { assert(false); }

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
        HostAddressPort host1;
        HostAddressPort host2;
        HostAddressPort host3;
        DummyEventLoop* eventLoop;
        StaticDomainNameResolver* resolver;
        MockConnectionFactory* connectionFactory;
        DummyTimerFactory* timerFactory;
        std::vector< std::shared_ptr<MockConnection> > connections;
        std::shared_ptr<Error> error;

};

CPPUNIT_TEST_SUITE_REGISTRATION(ConnectorTest);
