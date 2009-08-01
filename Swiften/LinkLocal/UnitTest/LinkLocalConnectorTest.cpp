#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/extensions/TestFactoryRegistry.h>

#include "Swiften/LinkLocal/LinkLocalConnector.h"
#include "Swiften/LinkLocal/LinkLocalService.h"
#include "Swiften/LinkLocal/DNSSD/DNSSDServiceID.h"
#include "Swiften/LinkLocal/DNSSD/DNSSDResolveHostnameQuery.h"
#include "Swiften/LinkLocal/DNSSD/Fake/FakeDNSSDQuerier.h"
#include "Swiften/EventLoop/DummyEventLoop.h"
#include "Swiften/Network/FakeConnection.h"

using namespace Swift;

class LinkLocalConnectorTest : public CppUnit::TestFixture {
		CPPUNIT_TEST_SUITE(LinkLocalConnectorTest);
		CPPUNIT_TEST(testConnect);
		CPPUNIT_TEST(testConnect_UnableToResolve);
		CPPUNIT_TEST(testConnect_UnableToConnect);
		CPPUNIT_TEST_SUITE_END();

	public:
		void setUp() {
			eventLoop = new DummyEventLoop();
			querier = boost::shared_ptr<FakeDNSSDQuerier>(
					new FakeDNSSDQuerier("rabbithole.local"));
			connection = boost::shared_ptr<FakeConnection>(new FakeConnection());
			connectFinished = false;
		}

		void tearDown() {
			delete eventLoop;
		}

		void testConnect() {
			boost::shared_ptr<LinkLocalConnector> 
					testling(createConnector("rabbithole.local", 1234));
			querier->setAddress("rabbithole.local", HostAddress("192.168.1.1"));

			testling->connect();
			eventLoop->processEvents();

			CPPUNIT_ASSERT(connectFinished);
			CPPUNIT_ASSERT(!connectError);
			CPPUNIT_ASSERT(connection->connectedTo);
			CPPUNIT_ASSERT_EQUAL(String(connection->connectedTo->getAddress().toString()), String("192.168.1.1"));
			CPPUNIT_ASSERT_EQUAL(connection->connectedTo->getPort(), 1234);
		}

		void testConnect_UnableToResolve() {
			boost::shared_ptr<LinkLocalConnector> 
					testling(createConnector("rabbithole.local", 1234));

			testling->connect();
			eventLoop->processEvents();

			CPPUNIT_ASSERT(connectFinished);
			CPPUNIT_ASSERT(connectError);
			CPPUNIT_ASSERT(!connection->connectedTo);
		}

		void testConnect_UnableToConnect() {
			boost::shared_ptr<LinkLocalConnector> 
					testling(createConnector("rabbithole.local", 1234));
			querier->setAddress("rabbithole.local", HostAddress("192.168.1.1"));
			connection->setError(Connection::ReadError);

			testling->connect();
			eventLoop->processEvents();

			CPPUNIT_ASSERT(connectFinished);
			CPPUNIT_ASSERT(connectError);
			CPPUNIT_ASSERT(!connection->connectedTo);
		}
	
	private:
		boost::shared_ptr<LinkLocalConnector> createConnector(const String& hostname, int port) {
			LinkLocalService service(
					DNSSDServiceID("myname", "local."),
					DNSSDResolveServiceQuery::Result(
						"myname._presence._tcp.local", hostname, port,
						LinkLocalServiceInfo().toTXTRecord()));
			boost::shared_ptr<LinkLocalConnector> result(
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
		boost::shared_ptr<FakeDNSSDQuerier> querier;
		boost::shared_ptr<FakeConnection> connection;
		bool connectFinished;
		bool connectError;
};

CPPUNIT_TEST_SUITE_REGISTRATION(LinkLocalConnectorTest);
