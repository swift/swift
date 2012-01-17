/*
 * Copyright (c) 2011 Kevin Smith
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#include <QA/Checker/IO.h>

#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/extensions/TestFactoryRegistry.h>

#include <boost/optional.hpp>
#include <boost/bind.hpp>
#include <boost/smart_ptr/make_shared.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/lexical_cast.hpp>

#include <Swiften/Base/Algorithm.h>
#include <Swiften/Network/Connection.h>
#include <Swiften/Network/ConnectionFactory.h>
#include <Swiften/Network/BOSHConnection.h>
#include <Swiften/Network/BOSHConnectionPool.h>
#include <Swiften/Network/HostAddressPort.h>
#include <Swiften/Network/StaticDomainNameResolver.h>
#include <Swiften/Network/DummyTimerFactory.h>
#include <Swiften/EventLoop/DummyEventLoop.h>
#include <Swiften/Parser/PlatformXMLParserFactory.h>



using namespace Swift;

typedef boost::shared_ptr<BOSHConnectionPool> PoolRef;


class BOSHConnectionPoolTest : public CppUnit::TestFixture {
	CPPUNIT_TEST_SUITE(BOSHConnectionPoolTest);
	CPPUNIT_TEST(testConnectionCount_OneWrite);
	CPPUNIT_TEST(testConnectionCount_TwoWrites);
	CPPUNIT_TEST(testConnectionCount_ThreeWrites);
	CPPUNIT_TEST(testConnectionCount_ThreeWrites_ManualConnect);
	CPPUNIT_TEST(testConnectionCount_ThreeWritesTwoReads);
	CPPUNIT_TEST(testSession);
	CPPUNIT_TEST(testWrite_Empty);
	CPPUNIT_TEST_SUITE_END();

	public:
		void setUp() {
			to = "wonderland.lit";
			path = "http-bind";
			port = "5280";
			sid = "MyShinySID";
			initial = "<body wait='60' "
					"inactivity='30' "
					"polling='5' "
					"requests='2' "
					"hold='1' "
					"maxpause='120' "
					"sid='" + sid + "' "
					"ver='1.6' "
					"from='wonderland.lit' "
					"xmlns='http://jabber.org/protocol/httpbind'/>";
			eventLoop = new DummyEventLoop();
			connectionFactory = new MockConnectionFactory(eventLoop);
			boshURL = URL("http", to, 5280, path);
			sessionTerminated = 0;
			sessionStarted = 0;
			initialRID = 2349876;
			xmppDataRead.clear();
			boshDataRead.clear();
			boshDataWritten.clear();
			resolver = new StaticDomainNameResolver(eventLoop);
			resolver->addAddress(to, HostAddress("127.0.0.1"));
			timerFactory = new DummyTimerFactory();
		}

		void tearDown() {
			eventLoop->processEvents();
			delete connectionFactory;
			delete resolver;
			delete timerFactory;
			delete eventLoop;
		}

		void testConnectionCount_OneWrite() {
			PoolRef testling = createTestling();
			CPPUNIT_ASSERT_EQUAL(st(1), connectionFactory->connections.size());
			eventLoop->processEvents();
			CPPUNIT_ASSERT_EQUAL(0, sessionStarted);
			readResponse(initial, connectionFactory->connections[0]);
			CPPUNIT_ASSERT_EQUAL(1, sessionStarted);
			CPPUNIT_ASSERT_EQUAL(st(1), connectionFactory->connections.size());
			testling->write(createSafeByteArray("<blah/>"));
			eventLoop->processEvents();
			CPPUNIT_ASSERT_EQUAL(st(1), connectionFactory->connections.size());
			CPPUNIT_ASSERT_EQUAL(1, sessionStarted);
		}

		void testConnectionCount_TwoWrites() {
			PoolRef testling = createTestling();
			CPPUNIT_ASSERT_EQUAL(st(1), connectionFactory->connections.size());
			eventLoop->processEvents();
			readResponse(initial, connectionFactory->connections[0]);
			eventLoop->processEvents();
			testling->write(createSafeByteArray("<blah/>"));
			eventLoop->processEvents();
			CPPUNIT_ASSERT_EQUAL(st(1), connectionFactory->connections.size());
			testling->write(createSafeByteArray("<bleh/>"));
			eventLoop->processEvents();
			eventLoop->processEvents();
			CPPUNIT_ASSERT_EQUAL(st(2), connectionFactory->connections.size());
		}

		void testConnectionCount_ThreeWrites() {
			PoolRef testling = createTestling();
			CPPUNIT_ASSERT_EQUAL(st(1), connectionFactory->connections.size());
			eventLoop->processEvents();
			readResponse(initial, connectionFactory->connections[0]);
			testling->restartStream();
			readResponse("<body/>", connectionFactory->connections[0]);
			testling->restartStream();
			readResponse("<body/>", connectionFactory->connections[0]);
			testling->write(createSafeByteArray("<blah/>"));
			testling->write(createSafeByteArray("<bleh/>"));
			testling->write(createSafeByteArray("<bluh/>"));
			eventLoop->processEvents();
			CPPUNIT_ASSERT(st(2) >= connectionFactory->connections.size());
		}

		void testConnectionCount_ThreeWrites_ManualConnect() {
			connectionFactory->autoFinishConnect = false;
			PoolRef testling = createTestling();
			CPPUNIT_ASSERT_EQUAL(st(1), connectionFactory->connections.size());
			CPPUNIT_ASSERT_EQUAL(st(0), boshDataWritten.size()); /* Connection not connected yet, can't send data */

			connectionFactory->connections[0]->onConnectFinished(false);
			eventLoop->processEvents();
			CPPUNIT_ASSERT_EQUAL(st(1), boshDataWritten.size()); /* Connection finished, stream header sent */

			readResponse(initial, connectionFactory->connections[0]);
			eventLoop->processEvents();
			CPPUNIT_ASSERT_EQUAL(st(1), connectionFactory->connections.size());
			CPPUNIT_ASSERT_EQUAL(st(1), boshDataWritten.size()); /* Don't respond to initial data with a holding call */

			testling->restartStream();
			eventLoop->processEvents();
			readResponse("<body/>", connectionFactory->connections[0]);
			eventLoop->processEvents();
			testling->restartStream();
			eventLoop->processEvents();


			testling->write(createSafeByteArray("<blah/>"));
			eventLoop->processEvents();
			CPPUNIT_ASSERT_EQUAL(st(2), connectionFactory->connections.size());
			CPPUNIT_ASSERT_EQUAL(st(3), boshDataWritten.size()); /* New connection isn't up yet. */

			connectionFactory->connections[1]->onConnectFinished(false);
			eventLoop->processEvents();
			CPPUNIT_ASSERT_EQUAL(st(4), boshDataWritten.size()); /* New connection ready. */

			testling->write(createSafeByteArray("<bleh/>"));
			eventLoop->processEvents();
			testling->write(createSafeByteArray("<bluh/>"));
			CPPUNIT_ASSERT_EQUAL(st(4), boshDataWritten.size()); /* New data can't be sent, no free connections. */
			eventLoop->processEvents();
			CPPUNIT_ASSERT(st(2) >= connectionFactory->connections.size());
		}

		void testConnectionCount_ThreeWritesTwoReads() {
			boost::shared_ptr<MockConnection> c0;
			boost::shared_ptr<MockConnection> c1;
			long rid = initialRID;

			PoolRef testling = createTestling();
			CPPUNIT_ASSERT_EQUAL(st(1), connectionFactory->connections.size());
			c0 = connectionFactory->connections[0];
			eventLoop->processEvents();
			CPPUNIT_ASSERT_EQUAL(st(1), boshDataWritten.size()); /* header*/

			rid++;
			readResponse(initial, c0);
			CPPUNIT_ASSERT_EQUAL(st(1), boshDataWritten.size());
			CPPUNIT_ASSERT_EQUAL(st(1), connectionFactory->connections.size());
			CPPUNIT_ASSERT(!c0->pending);

			rid++;
			testling->restartStream();
			eventLoop->processEvents();
			readResponse("<body/>", connectionFactory->connections[0]);

			rid++;
			testling->write(createSafeByteArray("<blah/>"));
			eventLoop->processEvents();
			CPPUNIT_ASSERT_EQUAL(st(2), connectionFactory->connections.size()); /* 0 was waiting for response, open and send on 1 */
			CPPUNIT_ASSERT_EQUAL(st(4), boshDataWritten.size()); /* data */
			c1 = connectionFactory->connections[1];
			std::string fullBody = "<body rid='" + boost::lexical_cast<std::string>(rid) + "' sid='" + sid + "' xmlns='http://jabber.org/protocol/httpbind'><blah/></body>"; /* check empty write */
			CPPUNIT_ASSERT_EQUAL(fullBody, lastBody());
			CPPUNIT_ASSERT(c0->pending);
			CPPUNIT_ASSERT(c1->pending);


			rid++;
			readResponse("<body xmlns='http://jabber.org/protocol/httpbind'><message><splatploing/></message></body>", c0); /* Doesn't include necessary attributes - as the support is improved this'll start to fail */
			eventLoop->processEvents();
			CPPUNIT_ASSERT(!c0->pending);
			CPPUNIT_ASSERT(c1->pending);
			CPPUNIT_ASSERT_EQUAL(st(4), boshDataWritten.size()); /* don't send empty in [0], still have [1] waiting */
			CPPUNIT_ASSERT_EQUAL(st(2), connectionFactory->connections.size());

			rid++;
			readResponse("<body xmlns='http://jabber.org/protocol/httpbind'><message><splatploing><blittlebarg/></splatploing></message></body>", c1);
			eventLoop->processEvents();
			CPPUNIT_ASSERT(!c1->pending);
			CPPUNIT_ASSERT(c0->pending);
			CPPUNIT_ASSERT_EQUAL(st(5), boshDataWritten.size()); /* empty to make room */
			CPPUNIT_ASSERT_EQUAL(st(2), connectionFactory->connections.size());

			rid++;
			testling->write(createSafeByteArray("<bleh/>"));
			eventLoop->processEvents();
			CPPUNIT_ASSERT(c0->pending);
			CPPUNIT_ASSERT(c1->pending);
			CPPUNIT_ASSERT_EQUAL(st(6), boshDataWritten.size()); /* data */

			rid++;
			testling->write(createSafeByteArray("<bluh/>"));
			CPPUNIT_ASSERT(c0->pending);
			CPPUNIT_ASSERT(c1->pending);
			CPPUNIT_ASSERT_EQUAL(st(6), boshDataWritten.size()); /* Don't send data, no room */
			eventLoop->processEvents();
			CPPUNIT_ASSERT_EQUAL(st(2), connectionFactory->connections.size());
		}

		void testSession() {
			to = "prosody.doomsong.co.uk";
			resolver->addAddress("prosody.doomsong.co.uk", HostAddress("127.0.0.1"));
			path = "http-bind/";
			boshURL = URL("http", to, 5280, path);

			PoolRef testling = createTestling();
			CPPUNIT_ASSERT_EQUAL(st(1), connectionFactory->connections.size());
			eventLoop->processEvents();
			CPPUNIT_ASSERT_EQUAL(st(1), boshDataWritten.size()); /* header*/
			CPPUNIT_ASSERT_EQUAL(st(1), connectionFactory->connections.size());

			std::string response = "<body authid='743da605-4c2e-4de1-afac-ac040dd4a940' xmpp:version='1.0' xmlns:stream='http://etherx.jabber.org/streams' xmlns:xmpp='urn:xmpp:xbosh' inactivity='60' wait='60' polling='5' secure='true' hold='1' from='prosody.doomsong.co.uk' ver='1.6' sid='743da605-4c2e-4de1-afac-ac040dd4a940' requests='2' xmlns='http://jabber.org/protocol/httpbind'><stream:features><auth xmlns='http://jabber.org/features/iq-auth'/><mechanisms xmlns='urn:ietf:params:xml:ns:xmpp-sasl'><mechanism>SCRAM-SHA-1</mechanism><mechanism>DIGEST-MD5</mechanism></mechanisms></stream:features></body>";
			readResponse(response, connectionFactory->connections[0]);
			eventLoop->processEvents();
			CPPUNIT_ASSERT_EQUAL(st(1), boshDataWritten.size());
			CPPUNIT_ASSERT_EQUAL(st(1), connectionFactory->connections.size());

			std::string send = "<auth xmlns=\"urn:ietf:params:xml:ns:xmpp-sasl\" mechanism=\"SCRAM-SHA-1\">biwsbj1hZG1pbixyPWZhOWE5ZDhiLWZmMDctNGE4Yy04N2E3LTg4YWRiNDQxZGUwYg==</auth>";
			testling->write(createSafeByteArray(send));
			eventLoop->processEvents();
			CPPUNIT_ASSERT_EQUAL(st(2), boshDataWritten.size());
			CPPUNIT_ASSERT_EQUAL(st(1), connectionFactory->connections.size());

			response = "<body xmlns='http://jabber.org/protocol/httpbind' sid='743da605-4c2e-4de1-afac-ac040dd4a940' xmlns:stream = 'http://etherx.jabber.org/streams'><challenge xmlns='urn:ietf:params:xml:ns:xmpp-sasl'>cj1mYTlhOWQ4Yi1mZjA3LTRhOGMtODdhNy04OGFkYjQ0MWRlMGJhZmZlMWNhMy1mMDJkLTQ5NzEtYjkyNS0yM2NlNWQ2MDQyMjYscz1OVGd5WkdWaFptTXRaVE15WXkwMFpXUmhMV0ZqTURRdFpqYzRNbUppWmpGa1pqWXgsaT00MDk2</challenge></body>";
			readResponse(response, connectionFactory->connections[0]);
			eventLoop->processEvents();
			CPPUNIT_ASSERT_EQUAL(st(2), boshDataWritten.size());
			CPPUNIT_ASSERT_EQUAL(st(1), connectionFactory->connections.size());

			send = "<response xmlns=\"urn:ietf:params:xml:ns:xmpp-sasl\">Yz1iaXdzLHI9ZmE5YTlkOGItZmYwNy00YThjLTg3YTctODhhZGI0NDFkZTBiYWZmZTFjYTMtZjAyZC00OTcxLWI5MjUtMjNjZTVkNjA0MjI2LHA9aU11NWt3dDN2VWplU2RqL01Jb3VIRldkZjBnPQ==</response>";
			testling->write(createSafeByteArray(send));
			eventLoop->processEvents();
			CPPUNIT_ASSERT_EQUAL(st(3), boshDataWritten.size());
			CPPUNIT_ASSERT_EQUAL(st(1), connectionFactory->connections.size());

			response = "<body xmlns='http://jabber.org/protocol/httpbind' sid='743da605-4c2e-4de1-afac-ac040dd4a940' xmlns:stream = 'http://etherx.jabber.org/streams'><success xmlns='urn:ietf:params:xml:ns:xmpp-sasl'>dj1YNmNBY3BBOWxHNjNOOXF2bVQ5S0FacERrVm89</success></body>";
			readResponse(response, connectionFactory->connections[0]);
			eventLoop->processEvents();
			CPPUNIT_ASSERT_EQUAL(st(3), boshDataWritten.size());
			CPPUNIT_ASSERT_EQUAL(st(1), connectionFactory->connections.size());
		}

		void testWrite_Empty() {
			boost::shared_ptr<MockConnection> c0;

			PoolRef testling = createTestling();
			CPPUNIT_ASSERT_EQUAL(st(1), connectionFactory->connections.size());
			c0 = connectionFactory->connections[0];

			readResponse(initial, c0);
			eventLoop->processEvents();
			CPPUNIT_ASSERT_EQUAL(st(1), boshDataWritten.size()); /* Shouldn't have sent anything extra */
			eventLoop->processEvents();
			testling->restartStream();
			eventLoop->processEvents();
			CPPUNIT_ASSERT_EQUAL(st(2), boshDataWritten.size());
			readResponse("<body></body>", c0);
			eventLoop->processEvents();
			CPPUNIT_ASSERT_EQUAL(st(3), boshDataWritten.size());
			std::string fullBody = "<body rid='" + boost::lexical_cast<std::string>(initialRID + 2) + "' sid='" + sid + "' xmlns='http://jabber.org/protocol/httpbind'></body>";
			std::string response = boshDataWritten[2];
			size_t bodyPosition = response.find("\r\n\r\n");
			CPPUNIT_ASSERT_EQUAL(fullBody, response.substr(bodyPosition+4));


		}

	private:

		PoolRef createTestling() {
			BOSHConnectionPool* a = new BOSHConnectionPool(boshURL, resolver, connectionFactory, &parserFactory, static_cast<TLSContextFactory*>(NULL), timerFactory, eventLoop, to, initialRID, URL(), SafeString(""), SafeString(""));
			PoolRef pool(a);
			//FIXME: Remko - why does the above work, but the below fail?
			//PoolRef pool = boost::make_shared<BOSHConnectionPool>(boshURL, resolver, connectionFactory, &parserFactory, static_cast<TLSContextFactory*>(NULL), timerFactory, eventLoop, to, initialRID, URL(), SafeString(""), SafeString(""));
			pool->onXMPPDataRead.connect(boost::bind(&BOSHConnectionPoolTest::handleXMPPDataRead, this, _1));
			pool->onBOSHDataRead.connect(boost::bind(&BOSHConnectionPoolTest::handleBOSHDataRead, this, _1));
			pool->onBOSHDataWritten.connect(boost::bind(&BOSHConnectionPoolTest::handleBOSHDataWritten, this, _1));
			pool->onSessionStarted.connect(boost::bind(&BOSHConnectionPoolTest::handleSessionStarted, this));
			pool->onSessionTerminated.connect(boost::bind(&BOSHConnectionPoolTest::handleSessionTerminated, this));
			eventLoop->processEvents();
			eventLoop->processEvents();
			return pool;
		}

		std::string lastBody() {
			std::string response = boshDataWritten[boshDataWritten.size() - 1];
			size_t bodyPosition = response.find("\r\n\r\n");
			return response.substr(bodyPosition+4);
		}

		size_t st(int val) {
			return static_cast<size_t>(val);
		}

		void handleXMPPDataRead(const SafeByteArray& d) {
			xmppDataRead.push_back(safeByteArrayToString(d));
		}

		void handleBOSHDataRead(const SafeByteArray& d) {
			boshDataRead.push_back(safeByteArrayToString(d));
		}

		void handleBOSHDataWritten(const SafeByteArray& d) {
			boshDataWritten.push_back(safeByteArrayToString(d));
		}


		void handleSessionStarted() {
			sessionStarted++;
		}

		void handleSessionTerminated() {
			sessionTerminated++;
		}

		struct MockConnection : public Connection {
			public:
				MockConnection(const std::vector<HostAddressPort>& failingPorts, EventLoop* eventLoop, bool autoFinishConnect) : eventLoop(eventLoop), failingPorts(failingPorts), disconnected(false), pending(false), autoFinishConnect(autoFinishConnect) {
				}

				void listen() { assert(false); }

				void connect(const HostAddressPort& address) {
					hostAddressPort = address;
					bool fail = std::find(failingPorts.begin(), failingPorts.end(), address) != failingPorts.end();
					if (autoFinishConnect) {
						eventLoop->postEvent(boost::bind(boost::ref(onConnectFinished), fail));
					}
				}

				HostAddressPort getLocalAddress() const { return HostAddressPort(); }

				void disconnect() {
					disconnected = true;
					onDisconnected(boost::optional<Connection::Error>());
				}

				void write(const SafeByteArray& d) {
					append(dataWritten, d);
					pending = true;
				}

				EventLoop* eventLoop;
				boost::optional<HostAddressPort> hostAddressPort;
				std::vector<HostAddressPort> failingPorts;
				ByteArray dataWritten;
				bool disconnected;
				bool pending;
				bool autoFinishConnect;
		};

		struct MockConnectionFactory : public ConnectionFactory {
			MockConnectionFactory(EventLoop* eventLoop, bool autoFinishConnect = true) : eventLoop(eventLoop), autoFinishConnect(autoFinishConnect) {
			}

			boost::shared_ptr<Connection> createConnection() {
				boost::shared_ptr<MockConnection> connection = boost::make_shared<MockConnection>(failingPorts, eventLoop, autoFinishConnect);
				connections.push_back(connection);
				return connection;
			}

			EventLoop* eventLoop;
			std::vector< boost::shared_ptr<MockConnection> > connections;
			std::vector<HostAddressPort> failingPorts;
			bool autoFinishConnect;
		};

		void readResponse(const std::string& response, boost::shared_ptr<MockConnection> connection) {
			connection->pending = false;
			boost::shared_ptr<SafeByteArray> data1 = boost::make_shared<SafeByteArray>(createSafeByteArray(
				"HTTP/1.1 200 OK\r\n"
				"Content-Type: text/xml; charset=utf-8\r\n"
				"Access-Control-Allow-Origin: *\r\n"
				"Access-Control-Allow-Headers: Content-Type\r\n"
				"Content-Length: "));
			connection->onDataRead(data1);
			boost::shared_ptr<SafeByteArray> data2 = boost::make_shared<SafeByteArray>(createSafeByteArray(boost::lexical_cast<std::string>(response.size())));
			connection->onDataRead(data2);
			boost::shared_ptr<SafeByteArray> data3 = boost::make_shared<SafeByteArray>(createSafeByteArray("\r\n\r\n"));
			connection->onDataRead(data3);
			boost::shared_ptr<SafeByteArray> data4 = boost::make_shared<SafeByteArray>(createSafeByteArray(response));
			connection->onDataRead(data4);
		}

		std::string fullRequestFor(const std::string& data) {
			std::string body = data;
			std::string result = "POST /" + path + " HTTP/1.1\r\n"
						+ "Host: " + to + ":" + port + "\r\n"
						+ "Content-Type: text/xml; charset=utf-8\r\n"
						+ "Content-Length: " + boost::lexical_cast<std::string>(body.size()) + "\r\n\r\n"
						+ body;
			return result;
		}

	private:
		URL boshURL;
		DummyEventLoop* eventLoop;
		MockConnectionFactory* connectionFactory;
		std::vector<std::string> xmppDataRead;
		std::vector<std::string> boshDataRead;
		std::vector<std::string> boshDataWritten;
		PlatformXMLParserFactory parserFactory;
		StaticDomainNameResolver* resolver;
		TimerFactory* timerFactory;
		std::string to;
		std::string path;
		std::string port;
		std::string sid;
		std::string initial;
		long initialRID;
		int sessionStarted;
		int sessionTerminated;

};


CPPUNIT_TEST_SUITE_REGISTRATION(BOSHConnectionPoolTest);
