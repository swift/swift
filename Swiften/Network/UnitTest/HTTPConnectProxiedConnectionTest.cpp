/*
 * Copyright (c) 2010-2016 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#include <memory>

#include <boost/algorithm/string.hpp>
#include <boost/bind.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/optional.hpp>

#include <QA/Checker/IO.h>

#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/extensions/TestFactoryRegistry.h>

#include <Swiften/Base/Algorithm.h>
#include <Swiften/Base/Log.h>
#include <Swiften/EventLoop/DummyEventLoop.h>
#include <Swiften/Network/Connection.h>
#include <Swiften/Network/ConnectionFactory.h>
#include <Swiften/Network/DummyTimerFactory.h>
#include <Swiften/Network/HTTPConnectProxiedConnection.h>
#include <Swiften/Network/HTTPTrafficFilter.h>
#include <Swiften/Network/HostAddressPort.h>
#include <Swiften/Network/StaticDomainNameResolver.h>

using namespace Swift;

namespace {
    class ExampleHTTPTrafficFilter : public HTTPTrafficFilter {
        public:
            ExampleHTTPTrafficFilter() {}
            virtual ~ExampleHTTPTrafficFilter() {}

            virtual std::vector<std::pair<std::string, std::string> > filterHTTPResponseHeader(const std::string& /* statusLine */, const std::vector<std::pair<std::string, std::string> >& response) {
                filterResponses.push_back(response);
                SWIFT_LOG(debug) << std::endl;
                return filterResponseReturn;
            }

            std::vector<std::vector<std::pair<std::string, std::string> > > filterResponses;

            std::vector<std::pair<std::string, std::string> > filterResponseReturn;
    };

    class ProxyAuthenticationHTTPTrafficFilter : public HTTPTrafficFilter {
            static std::string to_lower(const std::string& str) {
                std::string lower = str;
                boost::algorithm::to_lower(lower);
                return lower;
            }

        public:
            ProxyAuthenticationHTTPTrafficFilter() {}
            virtual ~ProxyAuthenticationHTTPTrafficFilter() {}

            virtual std::vector<std::pair<std::string, std::string> > filterHTTPResponseHeader(const std::string& statusLine, const std::vector<std::pair<std::string, std::string> >& response) {
                std::vector<std::pair<std::string, std::string> > filterResponseReturn;
                std::vector<std::string> statusLineFields;
                boost::split(statusLineFields, statusLine, boost::is_any_of(" "), boost::token_compress_on);

                int statusCode = boost::lexical_cast<int>(statusLineFields[1]);
                if (statusCode == 407) {
                    for (const auto& field : response) {
                        if (to_lower(field.first) == to_lower("Proxy-Authenticate")) {
                            if (field.second.size() >= 6 && field.second.substr(0, 6) == " NTLM ") {
                                filterResponseReturn.push_back(std::pair<std::string, std::string>("Proxy-Authorization", "NTLM TlRMTVNTUAADAAAAGAAYAHIAAAAYABgAigAAABIAEgBIAAAABgAGAFoAAAASABIVNTUAADAAYAAAABAAEACiAAAANYKI4gUBKAoAAAAPTABBAEIAUwBNAE8ASwBFADMAXwBxAGEATABBAEIAUwBNAE8ASwBFADMA0NKq8HYYhj8AAAAAAAAAAAAAAAAAAAAAOIiih3mR+AkyM4r99sy1mdFonCu2ILODro1WTTrJ4b4JcXEzUBA2Ig=="));
                                return filterResponseReturn;
                            }
                            else if (field.second.size() >= 5 && field.second.substr(0, 5) == " NTLM") {
                                filterResponseReturn.push_back(std::pair<std::string, std::string>("Proxy-Authorization", "NTLM TlRMTVNTUAABAAAAt7II4gkACQAxAAAACQAJACgAAAVNTUAADAAFASgKAAAAD0xBQlNNT0tFM1dPUktHUk9VUA=="));
                                return filterResponseReturn;
                            }
                        }
                    }

                    return filterResponseReturn;
                }
                else {
                    return std::vector<std::pair<std::string, std::string> >();
                }
            }
    };
}

class HTTPConnectProxiedConnectionTest : public CppUnit::TestFixture {
        CPPUNIT_TEST_SUITE(HTTPConnectProxiedConnectionTest);
        CPPUNIT_TEST(testConnect_CreatesConnectionToProxy);
        CPPUNIT_TEST(testConnect_SendsConnectRequest);
        CPPUNIT_TEST(testConnect_ReceiveConnectResponse);
        CPPUNIT_TEST(testConnect_ReceiveConnectChunkedResponse);
        CPPUNIT_TEST(testConnect_ReceiveMalformedConnectResponse);
        CPPUNIT_TEST(testConnect_ReceiveErrorConnectResponse);
        CPPUNIT_TEST(testConnect_ReceiveDataAfterConnect);
        CPPUNIT_TEST(testWrite_AfterConnect);
        CPPUNIT_TEST(testDisconnect_AfterConnectRequest);
        CPPUNIT_TEST(testDisconnect_AfterConnect);
        CPPUNIT_TEST(testTrafficFilter);
        CPPUNIT_TEST(testTrafficFilterNoConnectionReuse);
        CPPUNIT_TEST_SUITE_END();

    public:
        void setUp() {
            proxyHost = "doo.bah";
            proxyPort = 1234;
            proxyHostAddress = HostAddressPort(HostAddress::fromString("1.1.1.1").get(), proxyPort);
            host = HostAddressPort(HostAddress::fromString("2.2.2.2").get(), 2345);
            eventLoop = new DummyEventLoop();
            resolver = new StaticDomainNameResolver(eventLoop);
            resolver->addAddress(proxyHost, proxyHostAddress.getAddress());
            timerFactory = new DummyTimerFactory();
            connectionFactory = new MockConnectionFactory(eventLoop);
            connectFinished = false;
            connectFinishedWithError = false;
            disconnected = false;
        }

        void tearDown() {
            delete timerFactory;
            delete connectionFactory;
            delete resolver;
            delete eventLoop;
        }

        void connect(HTTPConnectProxiedConnection::ref connection, const HostAddressPort& to) {
            connection->connect(to);
            eventLoop->processEvents();
            eventLoop->processEvents();
            eventLoop->processEvents();
        }

        void testConnect_CreatesConnectionToProxy() {
            HTTPConnectProxiedConnection::ref testling(createTestling());

            connect(testling, host);

            CPPUNIT_ASSERT_EQUAL(1, static_cast<int>(connectionFactory->connections.size()));
            CPPUNIT_ASSERT(connectionFactory->connections[0]->hostAddressPort);
            CPPUNIT_ASSERT(proxyHostAddress == *connectionFactory->connections[0]->hostAddressPort);
            CPPUNIT_ASSERT(!connectFinished);
        }

        void testConnect_SendsConnectRequest() {
            HTTPConnectProxiedConnection::ref testling(createTestling());

            connect(testling, HostAddressPort(HostAddress::fromString("2.2.2.2").get(), 2345));

            CPPUNIT_ASSERT_EQUAL(createByteArray("CONNECT 2.2.2.2:2345 HTTP/1.1\r\n\r\n"), connectionFactory->connections[0]->dataWritten);
        }

        void testConnect_ReceiveConnectResponse() {
            HTTPConnectProxiedConnection::ref testling(createTestling());
            connect(testling, HostAddressPort(HostAddress::fromString("2.2.2.2").get(), 2345));

            connectionFactory->connections[0]->onDataRead(createSafeByteArrayRef("HTTP/1.0 200 Connection established\r\n\r\n"));
            eventLoop->processEvents();

            CPPUNIT_ASSERT(connectFinished);
            CPPUNIT_ASSERT(!connectFinishedWithError);
            CPPUNIT_ASSERT(dataRead.empty());
        }

        void testConnect_ReceiveConnectChunkedResponse() {
            HTTPConnectProxiedConnection::ref testling(createTestling());
            connect(testling, HostAddressPort(HostAddress::fromString("2.2.2.2").get(), 2345));

            connectionFactory->connections[0]->onDataRead(createSafeByteArrayRef("HTTP/1.0 "));
            eventLoop->processEvents();
            connectionFactory->connections[0]->onDataRead(createSafeByteArrayRef("200 Connection established\r\n\r\n"));
            eventLoop->processEvents();

            CPPUNIT_ASSERT(connectFinished);
            CPPUNIT_ASSERT(!connectFinishedWithError);
            CPPUNIT_ASSERT(dataRead.empty());
        }


        void testConnect_ReceiveMalformedConnectResponse() {
            HTTPConnectProxiedConnection::ref testling(createTestling());
            connect(testling, HostAddressPort(HostAddress::fromString("2.2.2.2").get(), 2345));

            connectionFactory->connections[0]->onDataRead(createSafeByteArrayRef("FLOOP"));
            eventLoop->processEvents();

            CPPUNIT_ASSERT(connectFinished);
            CPPUNIT_ASSERT(connectFinishedWithError);
            CPPUNIT_ASSERT(connectionFactory->connections[0]->disconnected);
        }

        void testConnect_ReceiveErrorConnectResponse() {
            HTTPConnectProxiedConnection::ref testling(createTestling());
            connect(testling, HostAddressPort(HostAddress::fromString("2.2.2.2").get(), 2345));

            connectionFactory->connections[0]->onDataRead(createSafeByteArrayRef("HTTP/1.0 401 Unauthorized\r\n\r\n"));
            eventLoop->processEvents();

            CPPUNIT_ASSERT(connectFinished);
            CPPUNIT_ASSERT(connectFinishedWithError);
            CPPUNIT_ASSERT(connectionFactory->connections[0]->disconnected);
        }

        void testConnect_ReceiveDataAfterConnect() {
            HTTPConnectProxiedConnection::ref testling(createTestling());
            connect(testling, HostAddressPort(HostAddress::fromString("2.2.2.2").get(), 2345));
            connectionFactory->connections[0]->onDataRead(createSafeByteArrayRef("HTTP/1.0 200 Connection established\r\n\r\n"));
            eventLoop->processEvents();

            connectionFactory->connections[0]->onDataRead(createSafeByteArrayRef("abcdef"));

            CPPUNIT_ASSERT_EQUAL(createByteArray("abcdef"), dataRead);
        }

        void testWrite_AfterConnect() {
            HTTPConnectProxiedConnection::ref testling(createTestling());
            connect(testling, HostAddressPort(HostAddress::fromString("2.2.2.2").get(), 2345));
            connectionFactory->connections[0]->onDataRead(createSafeByteArrayRef("HTTP/1.0 200 Connection established\r\n\r\n"));
            eventLoop->processEvents();
            connectionFactory->connections[0]->dataWritten.clear();

            testling->write(createSafeByteArray("abcdef"));

            CPPUNIT_ASSERT_EQUAL(createByteArray("abcdef"), connectionFactory->connections[0]->dataWritten);
        }

        void testDisconnect_AfterConnectRequest() {
            HTTPConnectProxiedConnection::ref testling(createTestling());
            connect(testling, HostAddressPort(HostAddress::fromString("2.2.2.2").get(), 2345));

            testling->disconnect();

            CPPUNIT_ASSERT(connectionFactory->connections[0]->disconnected);
            CPPUNIT_ASSERT(disconnected);
            CPPUNIT_ASSERT(!disconnectedError);
        }

        void testDisconnect_AfterConnect() {
            HTTPConnectProxiedConnection::ref testling(createTestling());
            connect(testling, HostAddressPort(HostAddress::fromString("2.2.2.2").get(), 2345));
            connectionFactory->connections[0]->onDataRead(createSafeByteArrayRef("HTTP/1.0 200 Connection established\r\n\r\n"));
            eventLoop->processEvents();

            testling->disconnect();

            CPPUNIT_ASSERT(connectionFactory->connections[0]->disconnected);
            CPPUNIT_ASSERT(disconnected);
            CPPUNIT_ASSERT(!disconnectedError);
        }

        void testTrafficFilter() {
            HTTPConnectProxiedConnection::ref testling(createTestling());

            std::shared_ptr<ExampleHTTPTrafficFilter> httpTrafficFilter = std::make_shared<ExampleHTTPTrafficFilter>();

            testling->setHTTPTrafficFilter(httpTrafficFilter);
            connect(testling, HostAddressPort(HostAddress::fromString("2.2.2.2").get(), 2345));

            // set a default response so the server response is answered by the traffic filter
            httpTrafficFilter->filterResponseReturn.clear();
            httpTrafficFilter->filterResponseReturn.push_back(std::pair<std::string, std::string>("Authorization", "Negotiate a87421000492aa874209af8bc028"));

            connectionFactory->connections[0]->dataWritten.clear();

            // test chunked response
            connectionFactory->connections[0]->onDataRead(createSafeByteArrayRef(
                "HTTP/1.0 401 Unauthorized\r\n"));
            eventLoop->processEvents();
            connectionFactory->connections[0]->onDataRead(createSafeByteArrayRef(
                "WWW-Authenticate: Negotiate\r\n"
                "\r\n"));
            eventLoop->processEvents();


            // verify that the traffic filter got called and answered with its response
            CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), httpTrafficFilter->filterResponses.size());
            CPPUNIT_ASSERT_EQUAL(std::string("WWW-Authenticate"), httpTrafficFilter->filterResponses[0][0].first);

            // remove the default response from the traffic filter
            httpTrafficFilter->filterResponseReturn.clear();
            eventLoop->processEvents();

            // verify that the traffic filter answer is send over the wire
            CPPUNIT_ASSERT_EQUAL(createByteArray("CONNECT 2.2.2.2:2345 HTTP/1.1\r\nAuthorization: Negotiate a87421000492aa874209af8bc028\r\n\r\n"), connectionFactory->connections[1]->dataWritten);

            // verify that after without the default response, the traffic filter is skipped, authentication proceeds and traffic goes right through
            connectionFactory->connections[1]->dataWritten.clear();
            testling->write(createSafeByteArray("abcdef"));
            CPPUNIT_ASSERT_EQUAL(createByteArray("abcdef"), connectionFactory->connections[1]->dataWritten);
        }

        void testTrafficFilterNoConnectionReuse() {
            HTTPConnectProxiedConnection::ref testling = createTestling();

            std::shared_ptr<ProxyAuthenticationHTTPTrafficFilter> httpTrafficFilter = std::make_shared<ProxyAuthenticationHTTPTrafficFilter>();
            testling->setHTTPTrafficFilter(httpTrafficFilter);

            connect(testling, HostAddressPort(HostAddress::fromString("2.2.2.2").get(), 2345));

            // First HTTP CONNECT request assumes the proxy will work.
            CPPUNIT_ASSERT_EQUAL(createByteArray("CONNECT 2.2.2.2:2345 HTTP/1.1\r\n"
                                                "\r\n"), connectionFactory->connections[0]->dataWritten);

            // First reply presents initiator with authentication options.
            connectionFactory->connections[0]->onDataRead(createSafeByteArrayRef(
                "HTTP/1.0 407 ProxyAuthentication Required\r\n"
                "proxy-Authenticate: Negotiate\r\n"
                "Proxy-Authenticate: Kerberos\r\n"
                "proxy-Authenticate: NTLM\r\n"
                "\r\n"));
            eventLoop->processEvents();
            CPPUNIT_ASSERT_EQUAL(false, connectFinished);
            CPPUNIT_ASSERT_EQUAL(false, connectFinishedWithError);

            // The HTTP proxy responds with code 407, so the traffic filter should inject the authentication response on a new connection.
            CPPUNIT_ASSERT_EQUAL(createByteArray("CONNECT 2.2.2.2:2345 HTTP/1.1\r\n"
                                                "Proxy-Authorization: NTLM TlRMTVNTUAABAAAAt7II4gkACQAxAAAACQAJACgAAAVNTUAADAAFASgKAAAAD0xBQlNNT0tFM1dPUktHUk9VUA==\r\n"
                                                "\r\n"), connectionFactory->connections[1]->dataWritten);

            // The proxy responds with another authentication step.
            connectionFactory->connections[1]->onDataRead(createSafeByteArrayRef(
                "HTTP/1.0 407 ProxyAuthentication Required\r\n"
                "Proxy-Authenticate: NTLM TlRMTVNTUAACAAAAEAAQADgAAAA1goriluCDYHcYI/sAAAAAAAAAAFQAVABIAAAABQLODgAAAA9TAFAASQBSAEkAVAAxAEIAAgAQAFMAUABJAFIASQBUADEAQgABABAAUwBQAEkAUgBJAFQAMQBCAAQAEABzAHAAaQByAGkAdAAxAGIAAwAQAHMAcABpAHIAaQB0ADEAYgAAAAAA\r\n"
                "\r\n"));
            eventLoop->processEvents();
            CPPUNIT_ASSERT_EQUAL(false, connectFinished);
            CPPUNIT_ASSERT_EQUAL(false, connectFinishedWithError);

            // Last HTTP request that should succeed. Further traffic will go over the connection of this request.
            CPPUNIT_ASSERT_EQUAL(createByteArray("CONNECT 2.2.2.2:2345 HTTP/1.1\r\n"
                                                "Proxy-Authorization: NTLM TlRMTVNTUAADAAAAGAAYAHIAAAAYABgAigAAABIAEgBIAAAABgAGAFoAAAASABIVNTUAADAAYAAAABAAEACiAAAANYKI4gUBKAoAAAAPTABBAEIAUwBNAE8ASwBFADMAXwBxAGEATABBAEIAUwBNAE8ASwBFADMA0NKq8HYYhj8AAAAAAAAAAAAAAAAAAAAAOIiih3mR+AkyM4r99sy1mdFonCu2ILODro1WTTrJ4b4JcXEzUBA2Ig==\r\n"
                                                "\r\n"), connectionFactory->connections[2]->dataWritten);

            connectionFactory->connections[2]->onDataRead(createSafeByteArrayRef(
                "HTTP/1.0 200 OK\r\n"
                "\r\n"));
            eventLoop->processEvents();

            // The HTTP CONNECT proxy initialization finished without error.
            CPPUNIT_ASSERT_EQUAL(true, connectFinished);
            CPPUNIT_ASSERT_EQUAL(false, connectFinishedWithError);

            // Further traffic is written directly, without interception of the filter.
            connectionFactory->connections[2]->dataWritten.clear();
            testling->write(createSafeByteArray("This is some basic data traffic."));
            CPPUNIT_ASSERT_EQUAL(createByteArray("This is some basic data traffic."), connectionFactory->connections[2]->dataWritten);
        }

    private:
        HTTPConnectProxiedConnection::ref createTestling() {
            std::shared_ptr<HTTPConnectProxiedConnection> c = HTTPConnectProxiedConnection::create(resolver, connectionFactory, timerFactory, proxyHost, proxyPort, "", "");
            c->onConnectFinished.connect(boost::bind(&HTTPConnectProxiedConnectionTest::handleConnectFinished, this, _1));
            c->onDisconnected.connect(boost::bind(&HTTPConnectProxiedConnectionTest::handleDisconnected, this, _1));
            c->onDataRead.connect(boost::bind(&HTTPConnectProxiedConnectionTest::handleDataRead, this, _1));
            return c;
        }

        void handleConnectFinished(bool error) {
            connectFinished = true;
            connectFinishedWithError = error;
        }

        void handleDisconnected(const boost::optional<Connection::Error>& e) {
            disconnected = true;
            disconnectedError = e;
        }

        void handleDataRead(std::shared_ptr<SafeByteArray> d) {
            append(dataRead, *d);
        }

        struct MockConnection : public Connection {
            public:
                MockConnection(const std::vector<HostAddressPort>& failingPorts, EventLoop* eventLoop) : eventLoop(eventLoop), failingPorts(failingPorts), disconnected(false) {
                }

                void listen() { assert(false); }

                void connect(const HostAddressPort& address) {
                    hostAddressPort = address;
                    bool fail = std::find(failingPorts.begin(), failingPorts.end(), address) != failingPorts.end();
                    eventLoop->postEvent(boost::bind(boost::ref(onConnectFinished), fail));
                }

                HostAddressPort getLocalAddress() const { return HostAddressPort(); }
                HostAddressPort getRemoteAddress() const { return HostAddressPort(); }

                void disconnect() {
                    disconnected = true;
                    onDisconnected(boost::optional<Connection::Error>());
                }

                void write(const SafeByteArray& d) {
                    append(dataWritten, d);
                }

                EventLoop* eventLoop;
                boost::optional<HostAddressPort> hostAddressPort;
                std::vector<HostAddressPort> failingPorts;
                ByteArray dataWritten;
                bool disconnected;
        };

        struct MockConnectionFactory : public ConnectionFactory {
            MockConnectionFactory(EventLoop* eventLoop) : eventLoop(eventLoop) {
            }

            std::shared_ptr<Connection> createConnection() {
                std::shared_ptr<MockConnection> connection = std::make_shared<MockConnection>(failingPorts, eventLoop);
                connections.push_back(connection);
                SWIFT_LOG(debug) << "new connection created" << std::endl;
                return connection;
            }

            EventLoop* eventLoop;
            std::vector< std::shared_ptr<MockConnection> > connections;
            std::vector<HostAddressPort> failingPorts;
        };

    private:
        std::string proxyHost;
        HostAddressPort proxyHostAddress;
        int proxyPort;
        HostAddressPort host;
        DummyEventLoop* eventLoop;
        StaticDomainNameResolver* resolver;
        MockConnectionFactory* connectionFactory;
        TimerFactory* timerFactory;
        bool connectFinished;
        bool connectFinishedWithError;
        bool disconnected;
        boost::optional<Connection::Error> disconnectedError;
        ByteArray dataRead;
};

CPPUNIT_TEST_SUITE_REGISTRATION(HTTPConnectProxiedConnectionTest);
