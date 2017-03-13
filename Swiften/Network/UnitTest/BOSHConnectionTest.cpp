/*
 * Copyright (c) 2011-2017 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#include <memory>

#include <boost/bind.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/optional.hpp>

#include <QA/Checker/IO.h>

#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/extensions/TestFactoryRegistry.h>

#include <Swiften/Base/Algorithm.h>
#include <Swiften/EventLoop/DummyEventLoop.h>
#include <Swiften/Network/BOSHConnection.h>
#include <Swiften/Network/Connection.h>
#include <Swiften/Network/ConnectionFactory.h>
#include <Swiften/Network/DummyTimerFactory.h>
#include <Swiften/Network/HostAddressPort.h>
#include <Swiften/Network/StaticDomainNameResolver.h>
#include <Swiften/Parser/PlatformXMLParserFactory.h>
#include <Swiften/TLS/TLSOptions.h>

using namespace Swift;

class BOSHConnectionTest : public CppUnit::TestFixture {
    CPPUNIT_TEST_SUITE(BOSHConnectionTest);
    CPPUNIT_TEST(testHeader);
    CPPUNIT_TEST(testReadiness_ok);
    CPPUNIT_TEST(testReadiness_pending);
    CPPUNIT_TEST(testReadiness_disconnect);
    CPPUNIT_TEST(testReadiness_noSID);
    CPPUNIT_TEST(testWrite_Receive);
    CPPUNIT_TEST(testWrite_ReceiveTwice);
    CPPUNIT_TEST(testRead_Fragment);
    CPPUNIT_TEST(testHTTPRequest);
    CPPUNIT_TEST(testHTTPRequest_Empty);
    CPPUNIT_TEST(testTerminate);
    CPPUNIT_TEST(testTerminateWithAdditionalData);
    CPPUNIT_TEST_SUITE_END();

    public:
        void setUp() {
            eventLoop = new DummyEventLoop();
            connectionFactory = new MockConnectionFactory(eventLoop);
            resolver = new StaticDomainNameResolver(eventLoop);
            timerFactory = new DummyTimerFactory();
            tlsContextFactory = nullptr;
            connectFinished = false;
            disconnected = false;
            disconnectedError = false;
            sessionTerminatedError.reset();
            dataRead.clear();
        }

        void tearDown() {
            eventLoop->processEvents();
            delete connectionFactory;
            delete resolver;
            delete timerFactory;
            delete eventLoop;
        }

        void testHeader() {
            BOSHConnection::ref testling = createTestling();
            testling->connect();
            eventLoop->processEvents();
            testling->startStream("wonderland.lit", 1);
            std::string initial("<body wait='60' "
                                "inactivity='30' "
                                "polling='5' "
                                "requests='2' "
                                "hold='1' "
                                "maxpause='120' "
                                "sid='MyShinySID' "
                                "ver='1.6' "
                                "from='wonderland.lit' "
                                "xmlns='http://jabber.org/protocol/httpbind'/>");
            readResponse(initial, connectionFactory->connections[0]);
            CPPUNIT_ASSERT_EQUAL(std::string("MyShinySID"), sid);
            CPPUNIT_ASSERT(testling->isReadyToSend());
        }

        void testReadiness_ok() {
            BOSHConnection::ref testling = createTestling();
            testling->connect();
            eventLoop->processEvents();
            testling->setSID("blahhhhh");
            CPPUNIT_ASSERT(testling->isReadyToSend());
        }

        void testReadiness_pending() {
            BOSHConnection::ref testling = createTestling();
            testling->connect();
            eventLoop->processEvents();
            testling->setSID("mySID");
            CPPUNIT_ASSERT(testling->isReadyToSend());
            testling->write(createSafeByteArray("<mypayload/>"));
            CPPUNIT_ASSERT(!testling->isReadyToSend());
            readResponse("<body><blah/></body>", connectionFactory->connections[0]);
            CPPUNIT_ASSERT(testling->isReadyToSend());
        }

        void testReadiness_disconnect() {
            BOSHConnection::ref testling = createTestling();
            testling->connect();
            eventLoop->processEvents();
            testling->setSID("mySID");
            CPPUNIT_ASSERT(testling->isReadyToSend());
            connectionFactory->connections[0]->onDisconnected(boost::optional<Connection::Error>());
            CPPUNIT_ASSERT(!testling->isReadyToSend());
        }


        void testReadiness_noSID() {
            BOSHConnection::ref testling = createTestling();
            testling->connect();
            eventLoop->processEvents();
            CPPUNIT_ASSERT(!testling->isReadyToSend());
        }

        void testWrite_Receive() {
            BOSHConnection::ref testling = createTestling();
            testling->connect();
            eventLoop->processEvents();
            testling->setSID("mySID");
            testling->write(createSafeByteArray("<mypayload/>"));
            readResponse("<body><blah/></body>", connectionFactory->connections[0]);
            CPPUNIT_ASSERT_EQUAL(std::string("<blah/>"), byteArrayToString(dataRead));

        }

        void testWrite_ReceiveTwice() {
            BOSHConnection::ref testling = createTestling();
            testling->connect();
            eventLoop->processEvents();
            testling->setSID("mySID");
            testling->write(createSafeByteArray("<mypayload/>"));
            readResponse("<body><blah/></body>", connectionFactory->connections[0]);
            CPPUNIT_ASSERT_EQUAL(std::string("<blah/>"), byteArrayToString(dataRead));
            dataRead.clear();
            testling->write(createSafeByteArray("<mypayload2/>"));
            readResponse("<body><bleh/></body>", connectionFactory->connections[0]);
            CPPUNIT_ASSERT_EQUAL(std::string("<bleh/>"), byteArrayToString(dataRead));
        }

        void testRead_Fragment() {
            BOSHConnection::ref testling = createTestling();
            testling->connect();
            eventLoop->processEvents();
            CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), connectionFactory->connections.size());
            std::shared_ptr<MockConnection> connection = connectionFactory->connections[0];
            std::shared_ptr<SafeByteArray> data1 = std::make_shared<SafeByteArray>(createSafeByteArray(
                "HTTP/1.1 200 OK\r\n"
                "Content-Type: text/xml; charset=utf-8\r\n"
                "Access-Control-Allow-Origin: *\r\n"
                "Access-Control-Allow-Headers: Content-Type\r\n"
                "Content-Length: 64\r\n"));
            std::shared_ptr<SafeByteArray> data2 = std::make_shared<SafeByteArray>(createSafeByteArray(
                "\r\n<body xmlns='http://jabber.org/protocol/httpbind'>"
                "<bl"));
            std::shared_ptr<SafeByteArray> data3 = std::make_shared<SafeByteArray>(createSafeByteArray(
                "ah/>"
                "</body>"));
            connection->onDataRead(data1);
            connection->onDataRead(data2);
            CPPUNIT_ASSERT(dataRead.empty());
            connection->onDataRead(data3);
            CPPUNIT_ASSERT_EQUAL(std::string("<blah/>"), byteArrayToString(dataRead));
        }

        void testHTTPRequest() {
            std::string data = "<blah/>";
            std::string sid = "wigglebloom";
            std::string fullBody = "<body xmlns='http://jabber.org/protocol/httpbind' sid='" + sid + "' rid='20'>" + data + "</body>";
            std::pair<SafeByteArray, size_t> http = BOSHConnection::createHTTPRequest(createSafeByteArray(data), false, false, 20, sid, URL());
            CPPUNIT_ASSERT_EQUAL(fullBody.size(), http.second);
        }

        void testHTTPRequest_Empty() {
            std::string data = "";
            std::string sid = "wigglebloomsickle";
            std::string fullBody = "<body rid='42' sid='" + sid + "' xmlns='http://jabber.org/protocol/httpbind'>" + data + "</body>";
            std::pair<SafeByteArray, size_t> http = BOSHConnection::createHTTPRequest(createSafeByteArray(data), false, false, 42, sid, URL());
            CPPUNIT_ASSERT_EQUAL(fullBody.size(), http.second);
            std::string response = safeByteArrayToString(http.first);
            size_t bodyPosition = response.find("\r\n\r\n");
            CPPUNIT_ASSERT_EQUAL(fullBody, response.substr(bodyPosition+4));
        }

        void testTerminate() {
            BOSHConnection::ref testling = createTestling();
            testling->connect();
            eventLoop->processEvents();
            testling->startStream("localhost", 1);
            std::string initial("<body xmlns=\"http://jabber.org/protocol/httpbind\" "
                                    "condition=\"bad-request\" "
                                    "type=\"terminate\">"
                                "</body>");
            readResponse(initial, connectionFactory->connections[0]);
            CPPUNIT_ASSERT(sessionTerminatedError);
            CPPUNIT_ASSERT_EQUAL(BOSHError::BadRequest, sessionTerminatedError->getType());
            CPPUNIT_ASSERT_EQUAL(true, dataRead.empty());
        }

        // On a BOSH error no additional data may be emitted.
        void testTerminateWithAdditionalData() {
            BOSHConnection::ref testling = createTestling();
            testling->connect();
            eventLoop->processEvents();
            testling->startStream("localhost", 1);
            std::string initial("<body xmlns=\"http://jabber.org/protocol/httpbind\" "
                                    "condition=\"bad-request\" "
                                    "type=\"terminate\">"
                                    "<text>an error message</text>"
                                "</body>");
            readResponse(initial, connectionFactory->connections[0]);
            CPPUNIT_ASSERT(sessionTerminatedError);
            CPPUNIT_ASSERT_EQUAL(BOSHError::BadRequest, sessionTerminatedError->getType());
            CPPUNIT_ASSERT_EQUAL(true, dataRead.empty());
        }


    private:

        BOSHConnection::ref createTestling() {
            resolver->addAddress("wonderland.lit", HostAddress::fromString("127.0.0.1").get());
            Connector::ref connector = Connector::create("wonderland.lit", 5280, boost::optional<std::string>(), resolver, connectionFactory, timerFactory);
            BOSHConnection::ref c = BOSHConnection::create(URL("http", "wonderland.lit", 5280, "/http-bind"), connector, &parserFactory, tlsContextFactory, TLSOptions());
            c->onConnectFinished.connect(boost::bind(&BOSHConnectionTest::handleConnectFinished, this, _1));
            c->onDisconnected.connect(boost::bind(&BOSHConnectionTest::handleDisconnected, this, _1));
            c->onXMPPDataRead.connect(boost::bind(&BOSHConnectionTest::handleDataRead, this, _1));
            c->onSessionStarted.connect(boost::bind(&BOSHConnectionTest::handleSID, this, _1));
            c->onSessionTerminated.connect(boost::bind(&BOSHConnectionTest::handleSessionTerminated, this, _1));
            c->setRID(42);
            return c;
        }

        void handleConnectFinished(bool error) {
            connectFinished = true;
            connectFinishedWithError = error;
        }

        void handleDisconnected(bool e) {
            disconnected = true;
            disconnectedError = e;
        }

        void handleDataRead(const SafeByteArray& d) {
            append(dataRead, d);
        }

        void handleSID(const std::string& s) {
            sid = s;
        }

        void handleSessionTerminated(BOSHError::ref error) {
            sessionTerminatedError = error;
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
                return connection;
            }

            EventLoop* eventLoop;
            std::vector< std::shared_ptr<MockConnection> > connections;
            std::vector<HostAddressPort> failingPorts;
        };

        void readResponse(const std::string& response, std::shared_ptr<MockConnection> connection) {
            std::shared_ptr<SafeByteArray> data1 = std::make_shared<SafeByteArray>(createSafeByteArray(
                "HTTP/1.1 200 OK\r\n"
                "Content-Type: text/xml; charset=utf-8\r\n"
                "Access-Control-Allow-Origin: *\r\n"
                "Access-Control-Allow-Headers: Content-Type\r\n"
                "Content-Length: "));
            connection->onDataRead(data1);
            std::shared_ptr<SafeByteArray> data2 = std::make_shared<SafeByteArray>(createSafeByteArray(boost::lexical_cast<std::string>(response.size())));
            connection->onDataRead(data2);
            std::shared_ptr<SafeByteArray> data3 = std::make_shared<SafeByteArray>(createSafeByteArray("\r\n\r\n"));
            connection->onDataRead(data3);
            std::shared_ptr<SafeByteArray> data4 = std::make_shared<SafeByteArray>(createSafeByteArray(response));
            connection->onDataRead(data4);
        }


    private:
        DummyEventLoop* eventLoop;
        MockConnectionFactory* connectionFactory;
        bool connectFinished;
        bool connectFinishedWithError;
        bool disconnected;
        bool disconnectedError;
        BOSHError::ref sessionTerminatedError;
        ByteArray dataRead;
        PlatformXMLParserFactory parserFactory;
        StaticDomainNameResolver* resolver;
        TimerFactory* timerFactory;
        TLSContextFactory* tlsContextFactory;
        std::string sid;

};


CPPUNIT_TEST_SUITE_REGISTRATION(BOSHConnectionTest);
